<#
    Randomly uploads programs every 5 - 60 seconds to the light server.
    This demostrates how the light server can be easily programmed
    and also provides a mechanism for testing stability.

    Usage:
        1. You can change the colours that are used to generate by modifying
           the variable $coloursToUse/

    Kevin White
    13 April 2021
#>

Clear-Host

. "$PSScriptRoot\LDL-Server-API.ps1"


$currentTemperature = 28        # this could come from an online service

$ldlProgram = [LDLProgram]::new("Current temperature")

# do a little animation to make it a bit more interesting than just displaying the temperature
# you can make this far more interesting but this is just a demo
for($i = 3; $i -gt 0; $i--) {
    $ins = $ldlProgram.AddInstruction([SliderEffectLDLInstruction]::new(1, 2, $false, 0, $i*10, [Colour]::new(255,0,0), [Colour]::new(0,0,0)))
    $ins = $ldlProgram.AddInstruction([SliderEffectLDLInstruction]::new(1, 2, $true, 0, $i*10, [Colour]::new(255,0,0), [Colour]::new(0,0,0)))
}

# display temperature as a block pattern (temperature in range 0 - 100 as a percent)
$colours = [Collections.Generic.List[ColourQuantity]]::new()::new(2)
$colours.Add([ColourQuantity]::new([Colour]::new(255,0,0), $currentTemperature))
$colours.Add([ColourQuantity]::new([colour]::new(0,0,0), 100 - $currentTemperature))
$ins = $ldlProgram.AddInstruction([BlocksEffectLDLInstruction]::new(1, $colours))

# $ip = "192.168.6.77"    #   blank for UDP discovery
$ip = ""
[LDLHelper]::UploadLDLProgramVerbose($ldlProgram, "", 8888)