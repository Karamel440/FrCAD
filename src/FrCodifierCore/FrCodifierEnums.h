#pragma once

enum class FrObjectCode
{
    rainSewer,
    mainSewer,
    pressureSewer,

    underWater,
    groundWater,

    underHeating,
    groundHeating,

    underGaz,
    groundGaz,

    underOilPipeline,
    groundOilPipeline,
    underFuelOilPipeline,
    underAirPipeline,
    groundAirPipeline,
    underAshPipeline,
    groundAshPipeline,
    underSpecial,
    groundSpecial,

    underPhone,
    groundPhone,

    tsrCable,
    controlSignalCable,
    elevatorSignalCable,
    fireSignalCable,
    radioCable,
    groundRadioCable,
    otherLowCurrentCable,

    lowVoltage,
    groundLowVoltage,

    highVoltage6_10,
    highVoltage35,
    highVoltage110_330,

    directCurrentCable,

    electroDrainCable,
    groundElectroDrainCable,
    potentialEqualizingJumper,
    anodeConductor,

    underCableZone,
    groundCableZone
};