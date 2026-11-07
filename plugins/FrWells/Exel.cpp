#include "Exel.h"
#include "Tablet.h"
#include "Nomenclature.h"

#include <FrString.h>
#include <FrExceptions.h>
#include <FrParser.h>

#include <Windows.h>
#include <OpenXLSX/OpenXLSX.hpp>
#include <filesystem>
#include <thread>
#include <exception>
#include <wrl/client.h>
#include <oleauto.h>
#pragma comment(lib, "OleAut32.lib")

namespace fs = std::filesystem;
using namespace OpenXLSX;



class ComGuard {
public:
	ComGuard() {
		HRESULT result =
			CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);

		if (FAILED(result))
			throw FrException{
			L"Не удалось инициализировать com"
		};

	}

	~ComGuard() {
		CoUninitialize();
	}

	ComGuard(const ComGuard&) = delete;
	ComGuard& operator=(const ComGuard&) = delete;

	ComGuard(ComGuard&&) = delete;
	ComGuard& operator=(ComGuard&&) = delete;
};
 
//std::wstring findExel(const fs::path& archive) {
//	
//	fs::path path { L"D:\\Работа\\Успенского\\65942_М-У\\23291408.xlsx" };
//
//	OpenXLSX::XLDocument doc;
//	doc.open(toString(path.native()));
//
//	auto book = doc.workbook();
//
//	auto sheet = book.worksheet(4);
//
//	auto cell = sheet.cell(60,1);
//
//	return toWstring(cell.value().getString());
//}

fs::path findExplication( int nomenclature, const fs::path& archive ) {

    const auto filename = std::to_wstring( nomenclature );

    for ( const auto& entry : fs::recursive_directory_iterator { archive } ) {
        if ( !entry.is_regular_file() )
            continue;

        const auto extension = entry.path().extension();

        if ( extension != L".xls" && extension != L".XLS" )
            continue;

        if ( entry.path().stem().wstring() == filename )
            return entry.path();
    }

    throw FrException { L"Экспликация не найдена" };
}

fs::path FrExcel::convertToXLSX( const fs::path& file ) {

    const fs::path source = fs::absolute( file );

    if ( !fs::is_regular_file( source ) )
        throw FrException { L"Не найден исходный файл XLS" };

    if ( source.extension() != L".xls" &&
        source.extension() != L".XLS" )
        throw FrException { L"Исходный файл должен иметь формат XLS" };

    fs::path output = source;
    output.replace_extension( L".xlsx" );

    if ( fs::exists( output ) ) {
        if ( !fs::is_regular_file( output ) )
            throw FrException { L"Путь XLSX занят не файлом" };

        if ( fs::last_write_time( output ) < fs::last_write_time( source ) )
            throw FrException {
                L"Исходный XLS новее существующего XLSX. Требуется повторная конвертация"
            };

        // Продолжаем работу с ранее созданным XLSX, не перезаписывая его.
        return output;
    }

    std::exception_ptr error {};

    std::thread thr( [source, output, &error] {

        try {
            ComGuard guard {};

            Microsoft::WRL::ComPtr<IDispatch> excel;
            Microsoft::WRL::ComPtr<IDispatch> workbooks;
            Microsoft::WRL::ComPtr<IDispatch> workbook;

            auto invoke = [](
                IDispatch* object,
                const wchar_t* member,
                WORD flags,
                VARIANT* arguments = nullptr,
                UINT count = 0,
                VARIANT* returnedValue = nullptr
                ) noexcept -> HRESULT {

                    if ( !object )
                        return E_POINTER;

                    LPOLESTR name = const_cast<LPOLESTR>( member );
                    DISPID id {};

                    HRESULT result = object->GetIDsOfNames(
                        IID_NULL, &name, 1,
                        LOCALE_USER_DEFAULT, &id
                    );

                    if ( FAILED( result ) )
                        return result;

                    DISPPARAMS parameters {};
                    parameters.rgvarg = arguments;
                    parameters.cArgs = count;

                    DISPID propertyPut = DISPID_PROPERTYPUT;

                    if ( flags & DISPATCH_PROPERTYPUT ) {
                        parameters.rgdispidNamedArgs = &propertyPut;
                        parameters.cNamedArgs = 1;
                    }

                    return object->Invoke(
                        id, IID_NULL, LOCALE_USER_DEFAULT,
                        flags, &parameters, returnedValue,
                        nullptr, nullptr
                    );
                };

            auto check = [](
                HRESULT result,
                const wchar_t* message
                ) {
                    if ( FAILED( result ) )
                        throw FrException { message };
                };

            try {
                CLSID id {};

                check(
                    CLSIDFromProgID( L"Excel.Application", &id ),
                    L"Не удалось получить CLSID Excel"
                );

                check(
                    CoCreateInstance(
                        id, nullptr,
                        CLSCTX_LOCAL_SERVER,
                        IID_PPV_ARGS( excel.GetAddressOf() )
                    ),
                    L"Не удалось запустить Excel"
                );

                VARIANT disabled {};
                disabled.vt = VT_BOOL;
                disabled.boolVal = VARIANT_FALSE;

                check(
                    invoke(
                        excel.Get(), L"DisplayAlerts",
                        DISPATCH_PROPERTYPUT, &disabled, 1
                    ),
                    L"Не удалось отключить запросы Excel"
                );

                check(
                    invoke(
                        excel.Get(), L"EnableEvents",
                        DISPATCH_PROPERTYPUT, &disabled, 1
                    ),
                    L"Не удалось отключить события Excel"
                );

                VARIANT security {};
                security.vt = VT_I4;
                security.lVal = 3;

                check(
                    invoke(
                        excel.Get(), L"AutomationSecurity",
                        DISPATCH_PROPERTYPUT, &security, 1
                    ),
                    L"Не удалось отключить макросы"
                );

                // Получаем коллекцию Workbooks.

                VARIANT value {};

                HRESULT result = invoke(
                    excel.Get(), L"Workbooks",
                    DISPATCH_PROPERTYGET,
                    nullptr, 0, &value
                );

                if ( FAILED( result ) ||
                    value.vt != VT_DISPATCH ||
                    !value.pdispVal ) {

                    VariantClear( &value );

                    throw FrException {
                        L"Не удалось получить Excel.Workbooks"
                    };
                }

                workbooks = value.pdispVal;
                VariantClear( &value );

                // Открываем исходный XLS.

                VARIANT openArguments[ 3 ] {};

                openArguments[ 0 ].vt = VT_BOOL;
                openArguments[ 0 ].boolVal = VARIANT_TRUE;

                openArguments[ 1 ].vt = VT_I4;
                openArguments[ 1 ].lVal = 0;

                openArguments[ 2 ].vt = VT_BSTR;
                openArguments[ 2 ].bstrVal =
                    SysAllocString( source.c_str() );

                if ( !openArguments[ 2 ].bstrVal )
                    throw FrException {
                        L"Не удалось выделить память для пути XLS"
                };

                result = invoke(
                    workbooks.Get(), L"Open",
                    DISPATCH_METHOD,
                    openArguments, 3, &value
                );

                VariantClear( &openArguments[ 2 ] );

                if ( FAILED( result ) ||
                    value.vt != VT_DISPATCH ||
                    !value.pdispVal ) {

                    VariantClear( &value );

                    throw FrException {
                        L"Не удалось открыть книгу XLS"
                    };
                }

                workbook = value.pdispVal;
                VariantClear( &value );

                // Сохраняем в формате XLSX.

                VARIANT saveArguments[ 2 ] {};

                saveArguments[ 0 ].vt = VT_I4;
                saveArguments[ 0 ].lVal = 51;

                saveArguments[ 1 ].vt = VT_BSTR;
                saveArguments[ 1 ].bstrVal =
                    SysAllocString( output.c_str() );

                if ( !saveArguments[ 1 ].bstrVal )
                    throw FrException {
                        L"Не удалось выделить память для пути XLSX"
                };

                result = invoke(
                    workbook.Get(), L"SaveAs",
                    DISPATCH_METHOD,
                    saveArguments, 2
                );

                VariantClear( &saveArguments[ 1 ] );

                check(
                    result,
                    L"Не удалось сохранить книгу в XLSX"
                );
            }
            catch ( ... ) {
                error = std::current_exception();
            }

            // Закрываем книгу и завершаем Excel.

            VARIANT disabled {};
            disabled.vt = VT_BOOL;
            disabled.boolVal = VARIANT_FALSE;

            if ( excel ) {
                (void) invoke(
                    excel.Get(), L"DisplayAlerts",
                    DISPATCH_PROPERTYPUT, &disabled, 1
                );
            }

            HRESULT closeResult = S_OK;
            HRESULT quitResult = S_OK;

            if ( workbook ) {
                closeResult = invoke(
                    workbook.Get(), L"Close",
                    DISPATCH_METHOD, &disabled, 1
                );
            }

            workbook.Reset();
            workbooks.Reset();

            if ( excel ) {
                quitResult = invoke(
                    excel.Get(), L"Quit",
                    DISPATCH_METHOD
                );
            }

            excel.Reset();

            if ( !error ) {
                check(
                    closeResult,
                    L"XLSX сохранён, но не удалось закрыть книгу"
                );

                check(
                    quitResult,
                    L"XLSX сохранён, но не удалось завершить Excel"
                );
            }
        }
        catch ( ... ) {
            if ( !error )
                error = std::current_exception();
        }
        } );

    thr.join();

    if ( error )
        std::rethrow_exception( error );

    if ( !fs::is_regular_file( output ) )
        throw FrException {
            L"После конвертации файл XLSX не найден"
    };

    return output;
}

fs::path FrExcel::convertToXLS( const fs::path& directory ) {

    if ( !doc.isOpen() )
        throw FrException { L"Документ XLSX не открыт" };

    // Сначала сохраняем изменения OpenXLSX.
    doc.save();

    const fs::path source = fs::absolute( path_ );

    if ( !fs::is_regular_file( source ) )
        throw FrException { L"Не найден исходный файл XLSX" };

    if ( source.extension() != L".xlsx" )
        throw FrException { L"Исходный файл должен иметь формат XLSX" };

    const fs::path destination = fs::absolute( directory );

    if ( !fs::is_directory( destination ) )
        throw FrException { L"Не найдена папка назначения" };

    fs::path output = destination / source.filename();
    output.replace_extension( L".xls" );

    if ( fs::exists( output ) )
        throw FrException {
            L"Файл XLS с таким именем уже существует"
    };

    std::exception_ptr error {};

    std::thread thr( [source, output, &error] {

        try {
            ComGuard guard {};

            Microsoft::WRL::ComPtr<IDispatch> excel;
            Microsoft::WRL::ComPtr<IDispatch> workbooks;
            Microsoft::WRL::ComPtr<IDispatch> workbook;

            auto invoke = [](
                IDispatch* object,
                const wchar_t* member,
                WORD flags,
                VARIANT* arguments = nullptr,
                UINT count = 0,
                VARIANT* returnedValue = nullptr
                ) noexcept -> HRESULT {

                    if ( !object )
                        return E_POINTER;

                    LPOLESTR name = const_cast<LPOLESTR>( member );
                    DISPID id {};

                    HRESULT result = object->GetIDsOfNames(
                        IID_NULL, &name, 1,
                        LOCALE_USER_DEFAULT, &id
                    );

                    if ( FAILED( result ) )
                        return result;

                    DISPPARAMS parameters {};
                    parameters.rgvarg = arguments;
                    parameters.cArgs = count;

                    DISPID propertyPut = DISPID_PROPERTYPUT;

                    if ( flags & DISPATCH_PROPERTYPUT ) {
                        parameters.rgdispidNamedArgs = &propertyPut;
                        parameters.cNamedArgs = 1;
                    }

                    return object->Invoke(
                        id, IID_NULL, LOCALE_USER_DEFAULT,
                        flags, &parameters, returnedValue,
                        nullptr, nullptr
                    );
                };

            auto check = [](
                HRESULT result,
                const wchar_t* message
                ) {
                    if ( FAILED( result ) )
                        throw FrException { message };
                };

            try {
                CLSID id {};

                check(
                    CLSIDFromProgID( L"Excel.Application", &id ),
                    L"Не удалось получить CLSID Excel"
                );

                check(
                    CoCreateInstance(
                        id, nullptr,
                        CLSCTX_LOCAL_SERVER,
                        IID_PPV_ARGS( excel.GetAddressOf() )
                    ),
                    L"Не удалось запустить Excel"
                );

                VARIANT disabled {};
                disabled.vt = VT_BOOL;
                disabled.boolVal = VARIANT_FALSE;

                check(
                    invoke(
                        excel.Get(), L"DisplayAlerts",
                        DISPATCH_PROPERTYPUT, &disabled, 1
                    ),
                    L"Не удалось отключить запросы Excel"
                );

                check(
                    invoke(
                        excel.Get(), L"EnableEvents",
                        DISPATCH_PROPERTYPUT, &disabled, 1
                    ),
                    L"Не удалось отключить события Excel"
                );

                VARIANT security {};
                security.vt = VT_I4;
                security.lVal = 3;

                check(
                    invoke(
                        excel.Get(), L"AutomationSecurity",
                        DISPATCH_PROPERTYPUT, &security, 1
                    ),
                    L"Не удалось отключить макросы"
                );

                VARIANT value {};

                HRESULT result = invoke(
                    excel.Get(), L"Workbooks",
                    DISPATCH_PROPERTYGET,
                    nullptr, 0, &value
                );

                if ( FAILED( result ) ||
                    value.vt != VT_DISPATCH ||
                    !value.pdispVal ) {

                    VariantClear( &value );
                    throw FrException {
                        L"Не удалось получить Excel.Workbooks"
                    };
                }

                workbooks = value.pdispVal;
                VariantClear( &value );

                // Открываем XLSX.

                VARIANT openArguments[ 3 ] {};

                openArguments[ 0 ].vt = VT_BOOL;
                openArguments[ 0 ].boolVal = VARIANT_TRUE;

                openArguments[ 1 ].vt = VT_I4;
                openArguments[ 1 ].lVal = 0;

                openArguments[ 2 ].vt = VT_BSTR;
                openArguments[ 2 ].bstrVal =
                    SysAllocString( source.c_str() );

                if ( !openArguments[ 2 ].bstrVal )
                    throw FrException {
                        L"Не удалось выделить память для пути XLSX"
                };

                result = invoke(
                    workbooks.Get(), L"Open",
                    DISPATCH_METHOD,
                    openArguments, 3, &value
                );

                VariantClear( &openArguments[ 2 ] );

                if ( FAILED( result ) ||
                    value.vt != VT_DISPATCH ||
                    !value.pdispVal ) {

                    VariantClear( &value );
                    throw FrException {
                        L"Не удалось открыть книгу XLSX"
                    };
                }

                workbook = value.pdispVal;
                VariantClear( &value );

                // Сохраняем в формате XLS (Excel 97-2003).

                VARIANT saveArguments[ 2 ] {};

                saveArguments[ 0 ].vt = VT_I4;
                saveArguments[ 0 ].lVal = 56;

                saveArguments[ 1 ].vt = VT_BSTR;
                saveArguments[ 1 ].bstrVal =
                    SysAllocString( output.c_str() );

                if ( !saveArguments[ 1 ].bstrVal )
                    throw FrException {
                        L"Не удалось выделить память для пути XLS"
                };

                result = invoke(
                    workbook.Get(), L"SaveAs",
                    DISPATCH_METHOD,
                    saveArguments, 2
                );

                VariantClear( &saveArguments[ 1 ] );

                check(
                    result,
                    L"Не удалось сохранить книгу в XLS"
                );
            }
            catch ( ... ) {
                error = std::current_exception();
            }

            // Закрытие Excel.

            VARIANT disabled {};
            disabled.vt = VT_BOOL;
            disabled.boolVal = VARIANT_FALSE;

            if ( excel ) {
                (void) invoke(
                    excel.Get(), L"DisplayAlerts",
                    DISPATCH_PROPERTYPUT, &disabled, 1
                );
            }

            HRESULT closeResult = S_OK;
            HRESULT quitResult = S_OK;

            if ( workbook ) {
                closeResult = invoke(
                    workbook.Get(), L"Close",
                    DISPATCH_METHOD, &disabled, 1
                );
            }

            workbook.Reset();
            workbooks.Reset();

            if ( excel ) {
                quitResult = invoke(
                    excel.Get(), L"Quit",
                    DISPATCH_METHOD
                );
            }

            excel.Reset();

            if ( !error ) {
                check(
                    closeResult,
                    L"XLS сохранён, но не удалось закрыть книгу"
                );

                check(
                    quitResult,
                    L"XLS сохранён, но не удалось завершить Excel"
                );
            }
        }
        catch ( ... ) {
            if ( !error )
                error = std::current_exception();
        }
        } );

    thr.join();

    if ( error )
        std::rethrow_exception( error );

    return output;
}

bool FrExcel::isHeader( const XLRow& row ) {
    int expected = 1;

    for ( auto& cell : row.cells() ) {
        XLCellValue value = cell.value();

        if ( value.type() == XLValueType::Empty )
            continue;

        bool matches = false;

        if ( value.type() == XLValueType::Integer )
            matches = value.get<int64_t>() == expected;

        else if ( value.type() == XLValueType::Float )
            matches = value.get<double>() == expected;

        if ( !matches )
            return false;

        if ( ++expected == 13 )
            return true;
    }

    return false;
}

std::optional<XLRow> FrExcel::findWell( const Well& well ) {

    if ( !well.number )
        return std::nullopt;

    for ( const auto& name : workbook.worksheetNames() ) {

        auto sheet = workbook.worksheet( name );
        bool headerFound = false;

        for ( auto& row : sheet.rows() ) {

            if ( !headerFound ) {
                if ( isHeader( row ) )
                    headerFound = true;

                continue;
            }
            
            auto cell = row.findCell( 1 );

            if ( cell.empty() )
                continue;

            XLCellValue value = cell.value();
            std::wstring number;

			switch ( value.type() ) {

				case XLValueType::Integer:
				number = std::to_wstring(
					value.get<int64_t>()
				);
				break;

				case XLValueType::Float:
				{
					double n = value.get<double>();

					if ( std::isfinite( n ) &&
						std::to_wstring( static_cast<int64_t>( n ) ) == *well.number )
						return row;

					break;
				}

				case XLValueType::String:
				number = toWstring(
					value.get<std::string>()
				);
				break;

				default:
				continue;
			}

            if ( number == *well.number )
                return row;
        }
    }

    return std::nullopt;
}

void FrExcel::changeWell( XLRow& row, std::optional<double> height, int year ) {

	auto mark = row.findCell( 6 );
	auto status = row.findCell( 14 );
	std::wstring finalStatus;

	if ( height ) {
		mark.value() = *height;
		finalStatus = parse( toWstring( status.value().getString() ), true, year );
	}
	else {
		finalStatus = parse( toWstring( status.value().getString() ), false, year );
	}
	status.value() = toString( finalStatus );
}

void FrExcel::processWells( const std::vector<Well>& wells ) {
    for ( const auto& well : wells ) {
        auto row = findWell( well );
       
        if ( !row )
            continue;

        changeWell( *row, well.height, 2026 );
    }
    doc.save();
}
