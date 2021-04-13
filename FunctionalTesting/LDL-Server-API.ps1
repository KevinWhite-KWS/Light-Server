<#
    Simple API to generate LDL programs.

    Usage example (creates a program that clears all LEDs then sets them to red, green, and then blue - infinitly):

    . "$PSScriptRoot\LDL-Server-API.ps1"        #   include this script in your own

    $ldlProgram = [LDLProgram]::new("My simple program")                        #   program class that allows a single LDL program to be built
    $infiniteLoop = $ldlProgram.AddInstructon([LoopLDLInstruction]::new(0))     #   parameter 0 means this will be an infinite loop and this is the first instruction in the program
    $ins = $infiniteLoop.AddInstruction([ClearEffectLDLInstruction]::new(10))   #   clear all LEDs for 10 rendering frames - 1st in struction in the infinite loop
    $ins = $infiniteLoop.AddInstruction([SolidEffectLDLInstruction]::new(20, [Colour]::new(255,0,0)))   # set LEDs to red for 20 rendering frames
    $ins = $infiniteLoop.AddInstruction([SolidEffectLDLInstruction]::new(20, [Colour]::new(0,255,0)))   # set LEDs to green for 20 rendering frames
    $ins = $infiniteLoop.AddInstruction([SolidEffectLDLInstruction]::new(20, [Colour]::new(0,0,255)))   # set LEDs to blue for 20 rendering frames
    $currentProgram = $ldlProgram.SerializeToLDL()                              #   Serialize the program to LDL (JSON) - this can now be sent to the server

    Kevin White
    13 April 2021
#>

using namespace Newtonsoft.Json

<#
    .Description
    Stores RGB colour data.
    Kevin White
    5 Apr 2021
#>
class Colour {
    [byte]$red = 0
    [byte]$green = 0
    [byte]$blue = 0

    Colour([byte]$red, [byte]$green, [byte]$blue) {
        $this.red = $red
        $this.green = $green
        $this.blue = $blue
    }    
}

<#
    .Description
    Stores RGB colour data and a quantity value
    associated to that colour e.g. number of pixels.
    Kevin White
    5 Apr 2021
#>
class ColourQuantity {
    [Colour]$colour
    [byte]$quantity

    ColourQuantity([Colour]$colour, [byte]$quantity) {
        $this.colour = $colour
        $this.quantity = $quantity
    }
}

<#
    .Description
    Abstract base-class for a class that provides a serializer
    to serialize some basic data to LDL.  For example: a serializer
    to serialize a colour to LDL (6-digit hex).
    Kevin White
    5 Apr 2021
#>
class LDLSerializer {
    LDLSerializer ()
    {
        $type = $this.GetType()

        if ($type -eq [LDLSerializer])
        {
            throw("Class $type must be inherited")
        }
    }

    [string] SerializeToLDL()
    {
        throw("Must Override Method")
    }    
}

<#
    .Description
    Serializes a colour to LDL.  A colour is simply
    serialzied to a hexidecimal representation.  Example:
    red (255, 0, 0) becomes FF0000.
    Kevin White
    5 Apr 2021
#>
class ColourLDLSerializer : LDLSerializer {
    [Colour]$colour

    ColourLDLSerializer([Colour]$colour) {
        $this.colour = $colour
    }

    [string] SerializeToLDL()
    {
        $r = "{0:X2}" -f $this.colour.red
        $g = "{0:X2}" -f $this.colour.green
        $b = "{0:X2}" -f $this.colour.blue

        return "${r}${g}${b}"
    }     
}

<#
    .Description
    Serializes a number, to be a specified number of digits, as a
    hexidecimal representations.  For example: the number 10 to
    two digits becomes 0A.
    Kevin White
    5 Apr 2021
#>
class NumberLDLSerializer : LDLSerializer {
    [uint16]$value
    [byte]$numberOfDigits = 2

    NumberLDLSerializer([uint16]$value, [byte]$numberOfDigits) {
        $this.value = $value
        $this.numberOfDigits = $numberOfDigits
    }

    [string] SerializeToLDL()
    {
        $numDigits = $this.numberOfDigits

        return "{0:X${numDigits}}" -f $this.value
    } 
}

<#
    .Description
    Abstract base-class for any LDL instruction.
    5 Apr 2021
#>
class LDLInstruction {
    LDLInstruction ()
    {
        $type = $this.GetType()

        if ($type -eq [LDLInstruction])
        {
            throw("Class $type must be inherited")
        }
    }
}

<#
    .Description
    A repeat instruction in LDL.  NOTE: this is not intented to be
    instantiated directly.  Instead create a LoopLDLInstruction.  This
    one is required to ensure that the loops are serialzied
    correctly to JSON.
    (loops in JSON are serialized as:
    {       // output object
        "repeat": {     // inner object (this RepeatLDLInstruction)
            "times": 1,
            "instructions": [
                ...
            ]
        }
    }
    )
    5 Apr 2021
#>
class RepeatLdlInstruction {
    [byte]$times

    [Collections.Generic.List[LDLInstruction]]$instructions

    RepeatLdlInstruction() {
        $this.instructions = [Collections.Generic.List[LDLInstruction]]::new()
    }    
}
<#
    .Description
    A loop instruction in LDL.  If the times parameter is 0
    the loop will executed infinitly.
    5 Apr 2021
#>
class LoopLDLInstruction : LDLInstruction {
    $repeat = [RepeatLdlInstruction]::new()


    LoopLDLInstruction([byte]$times = 0) {
        $this.repeat.times = $times;
    }

    [Collections.Generic.List[LDLInstruction]]GetInstructions() {
        return $this.repeat.instructions
    }

    [LDLInstruction]AddInstruction([LDLInstruction]$instruction) {
        $this.repeat.instructions.Add($instruction)
        return $instruction
    }
}

<#
    .Description
    Abstract base-class for any LDL instruction that causes
    rendering to occur.  Each concrete class must provide
    an implementation of GetLDLSerializers which returns
    a collection of serializers to build the LDL.
    5 Apr 2021
#>
class EffectLDLInstruction : LDLInstruction {
    [JsonIgnoreAttribute()]
    hidden [byte]$duration

    [JsonIgnoreAttribute()]
    hidden [byte]$instructionNumber = 0

    hidden [string]$ins = ""

    EffectLDLInstruction ()
    {
        $type = $this.GetType()

        if ($type -eq [EffectLDLInstruction])
        {
            throw("Class $type must be inherited")
        }
    }    

    [Collections.Generic.List[LDLSerializer]] GetLDLSerializers()
    {
        throw("Must Override Method")
    }

    [byte]GetInstructionNumber() {
        return $this.instructionNumber
    }

    <#
    .Description
    Seralizes the instruction to LDL.  This will consist
    of the following:
        xx      (two digit hex instruction #)
        yy      (two digit frame duration)
        00      (reserved two digit hex)
        00      (reserved two digit hex)
        ins     (the particulars of the concrete instruction)
    5 Apr 2021
    #>
    SerializeToLDL() {
        # serialize xx (two digit hex instruction #)
        # $insNumberSerializer = [NumberLDLSerializer]::new($this.instructionNumber, 2)
        $insNumberSerializer = [NumberLDLSerializer]::new($this.GetInstructionNumber(), 2)
        $insNumberLdl = $insNumberSerializer.SerializeToLDL()
        $this.ins = $insNumberLdl
        # serialize yy (two digit frame duration)
        $durationNumberSerializer = [NumberLDLSerializer]::new($this.duration, 2)
        $durationLdl = $durationNumberSerializer.SerializeToLDL()
        # add in the four reserved digits)
        $this.ins += $durationLdl + "0000"

        # serialize the particulars of the concrete instruction by
        # getting the collection of serializers for that instruction
        $ldlSerializers = $this.GetLDLSerializers()
        for($i = 0; $i -lt $ldlSerializers.Count; $i++) {
            $ldlSerializer = $ldlSerializers[$i]
            $this.ins += $ldlSerializer.SerializeToLDL()
        }

    }
}

<#
    .Description
    Clear instruction (00).  Clears the LEDs by turning them all off.  
    This instruction is useful for resetting the state 
    of the LEDs after running a succession of effects.
    5 Apr 2021
#>
class ClearEffectLDLInstruction : EffectLDLInstruction {
    ClearEffectLDLInstruction([byte]$duration) {
        $this.instructionNumber = 0
        $this.duration = $duration
        $this.SerializeToLDL()
    }

    [Collections.Generic.List[LDLSerializer]] GetLDLSerializers()
    {
        return new-object Collections.Generic.List[LDLSerializer]
    }      
}

<#
    .Description
    Solid instruction (00).  Sets all LEDs to the same colour.  
    This effect is achieved in a single rendering frame.
    5 Apr 2021
#>
class SolidEffectLDLInstruction : EffectLDLInstruction {
    [JsonIgnoreAttribute()]
    [Colour]$colour

    SolidEffectLDLInstruction([byte]$duration, [Colour]$colour) {
        $this.instructionNumber = 1
        $this.duration = $duration
        $this.colour = $colour
        $this.SerializeToLDL()
    }

    [Collections.Generic.List[LDLSerializer]] GetLDLSerializers()
    {
        $serializers = new-object Collections.Generic.List[LDLSerializer]
        $colourSerialzier = [ColourLDLSerializer]::new($this.colour)
        $serializers.Add($colourSerialzier)

        return $serializers
    }      
}

<#
    .Description
    Pattern instruction (02).  Sets the first n LEDs to a pattern which 
    is then repeated along the entire length of LEDs.
    5 Apr 2021
#>
class PatternEffectLDLInstruction : EffectLDLInstruction {
    [JsonIgnoreAttribute()]
    [Collections.Generic.List[ColourQuantity]]$patternElements = [Collections.Generic.List[ColourQuantity]]::new()

    [byte]GetInstructionNumber() {
        return 2
    }

    PatternEffectLDLInstruction([byte]$duration, [ColourQuantity[]]$patternElements) {
        
        $this.duration = $duration
        
        $this.patternElements.AddRange($patternElements)
        $this.SerializeToLDL()
    }

    [Collections.Generic.List[LDLSerializer]] GetLDLSerializers()
    {
        $serializers = [Collections.Generic.List[LDLSerializer]]::new()

        if($null -ne $this.patternElements) {
            $serializers.Add([NumberLDLSerializer]::new($this.patternElements.Count, 2))

            $colourSerializers = [Collections.Generic.List[LDLSerializer]]::new()
            $this.patternElements | ForEach-Object {
                $patternElement = [ColourQuantity]$_
                $serializers.Add([NumberLDLSerializer]::new($patternElement.quantity, 2))
                $colourSerializers.Add([ColourLDLSerializer]::new($patternElement.colour))
            }
            $serializers.AddRange($colourSerializers)
        }        

        return $serializers
    }      
}

<#
    .Description
    Solid instruction (03).  Moves a slider (a length of pixels of a specified colour) gradually along the length of pixels.  
    The effect either starts at the ‘near’ side (those pixels closest to the controller) or otherwise the ‘far’ 
    side (those pixels furthest away from the controller).  There can be an optional leading ‘head’.  
    The head smooths the transition from the slider colour to the background.  
    Likewise, an optional ‘tail’ is also supported.
    5 Apr 2021
#>
class SliderEffectLDLInstruction : EffectLDLInstruction {
    [JsonIgnoreAttribute()]
    [byte]$sliderWidth = 2
    [JsonIgnoreAttribute()]
    [bool]$startNear = $true
    [JsonIgnoreAttribute()]
    [byte]$headLength = 25
    [JsonIgnoreAttribute()]
    [byte]$tailLength = 25
    [JsonIgnoreAttribute()]
    [Colour]$sliderColour = [Colour]::new(255, 0, 0)
    [JsonIgnoreAttribute()]
    [Colour]$backgroundColour = [Colour]::new(0,0,0)

    SliderEffectLDLInstruction([byte]$duration, [byte]$sliderWidth=2, 
        [bool]$startNear=$true, [byte]$headLength=25, 
        [byte]$tailLength=25, [Colour]$sliderColour, 
        [Colour]$backgroundColour) {

        $this.instructionNumber =3
        $this.duration = $duration

        $this.sliderWidth = $sliderWidth
        $this.startNear = $startNear
        $this.headLength = $headLength
        $this.tailLength = $tailLength
        $this.sliderColour = $sliderColour
        $this.backgroundColour = $backgroundColour

        $this.SerializeToLDL()
    }

    [Collections.Generic.List[LDLSerializer]] GetLDLSerializers()
    {
        $serializers = [Collections.Generic.List[LDLSerializer]]::new()

        $serializers.Add([NumberLDLSerializer]::new($this.sliderWidth, 2))
        $serializers.Add([NumberLDLSerializer]::new($this.startNear, 1))
        $serializers.Add([NumberLDLSerializer]::new($this.headLength, 2))
        $serializers.Add([NumberLDLSerializer]::new($this.tailLength, 2))
        $serializers.Add([ColourLDLSerializer]::new($this.sliderColour))
        $serializers.Add([ColourLDLSerializer]::new($this.backgroundColour))

        return $serializers
    }      
}

<#
    .Description
    Fade instruction (04).  Fades in or fades out the LEDs from the start colour to the end colour.  
    The number of rendered frames depends on the colour specified and the step value.
    5 Apr 2021
#>
class FadeEffectLDLInstruction : EffectLDLInstruction {
    [JsonIgnoreAttribute()]
    [byte]$stepValue = 10
    [JsonIgnoreAttribute()]
    [bool]$fadeOut = $false
    [JsonIgnoreAttribute()]
    [Colour]$startColour = [Colour]::new(0, 0, 0)
    [JsonIgnoreAttribute()]
    [Colour]$endColour = [Colour]::new(255,0,0)

    FadeEffectLDLInstruction([byte]$duration, [byte]$stepValue=10, 
        [bool]$fadeOut=$false, [Colour]$startColour, 
        [Colour]$endColour) {

        $this.instructionNumber = 4
        $this.duration = $duration

        $this.stepValue = $stepValue
        $this.fadeOut = $fadeOut
        $this.startColour = $startColour
        $this.endColour = $endColour

        $this.SerializeToLDL()
    }

    [Collections.Generic.List[LDLSerializer]] GetLDLSerializers()
    {
        $serializers = [Collections.Generic.List[LDLSerializer]]::new()

        $serializers.Add([NumberLDLSerializer]::new($this.stepValue, 2))
        $serializers.Add([NumberLDLSerializer]::new($this.fadeOut, 1))
        $serializers.Add([ColourLDLSerializer]::new($this.startColour))
        $serializers.Add([ColourLDLSerializer]::new($this.endColour))

        return $serializers
    }      
}

<#
    .Description
    Stochastic instruction (05).  Randomly sets the LEDs to two or more colours.
    5 Apr 2021
#>
class StochasticEffectLDLInstruction : EffectLDLInstruction {
    [JsonIgnoreAttribute()]
    [Collections.Generic.List[Colour]]$colours = [Collections.Generic.List[Colour]]::new()

    # StochasticEffectLDLInstruction([byte]$duration, [Collections.Generic.List[Colour]]$colours) {
    StochasticEffectLDLInstruction([byte]$duration, [Colour[]]$colours) {

        $this.instructionNumber = 5
        $this.duration = $duration

        # $this.colours = $colours
        $this.colours.AddRange($colours)

        $this.SerializeToLDL()
    }

    [Collections.Generic.List[LDLSerializer]] GetLDLSerializers()
    {
        $serializers = [Collections.Generic.List[LDLSerializer]]::new()

        if($this.colours -ne $null) {
            $serializers.Add([NumberLDLSerializer]::new($this.colours.Count, 2))

            $this.colours | ForEach-Object {
                $colour = $_
                $serializers.Add([ColourLDLSerializer]::new($colour))
            }
        }

        return $serializers
    }      
}

class BlocksEffectLDLInstruction : PatternEffectLDLInstruction {
    [byte]GetInstructionNumber() {
        return 6
    }

    BlocksEffectLDLInstruction([byte]$duration, [ColourQuantity[]]$patternElements) 
    : base($duration, $patternElements) {
    }    
}

<#
    .Description
    Rainbow instruction (07).  Renders a moving ‘rainbow’ 
    colour effect over a number of steps across a specified length.  
    The number of rendered frames in the animation effect is equivalent to the value of steps.
    7 Apr 2021
#>
class RainbowEffectLDLInstruction : EffectLDLInstruction {
    [JsonIgnoreAttribute()]
    [byte]$length = 10
    [JsonIgnoreAttribute()]
    [byte]$steps = 10
    [JsonIgnoreAttribute()]
    [bool]$startNear = $true

    [JsonIgnoreAttribute()]
    [Collections.Generic.List[Colour]]$colours = [Collections.Generic.List[Colour]]::new()

    RainbowEffectLDLInstruction([byte]$duration, [byte]$length = 10,
        [byte]$steps = 10, [bool]$startNear = $true,
        [Colour[]]$colours) {

        $this.instructionNumber = 7
        $this.duration = $duration

        # $this.colours = $colours
        $this.length = $length
        $this.steps = $steps
        $this.startNear = $startNear
        $this.colours.AddRange($colours)

        $this.SerializeToLDL()
    }

    [Collections.Generic.List[LDLSerializer]] GetLDLSerializers()
    {
        $serializers = [Collections.Generic.List[LDLSerializer]]::new()

        $serializers.Add([NumberLDLSerializer]::new($this.length, 2))
        $serializers.Add([NumberLDLSerializer]::new($this.steps, 2))
        $serializers.Add([NumberLDLSerializer]::new($this.startNear, 1))

        if($null -ne $this.colours) {
            $serializers.Add([NumberLDLSerializer]::new($this.colours.Count, 2))

            $this.colours | ForEach-Object {
                $colour = $_
                $serializers.Add([ColourLDLSerializer]::new($colour))
            }
        }

        return $serializers
    }      
}

<#
    .Description
    Create an LDL program that has one or more instructions.
    9 Apr 2021
#>
class LDLProgram {
    hidden [string]$name
    $instructions

    LDLProgram([string]$name) {
        $this.name = $name
        $this.instructions = New-Object Collections.Generic.List[LDLInstruction]
    }

    [LDLInstruction]AddInstructon([LDLInstruction]$instruction) {
        $this.instructions.Add($instruction)
        return $instruction
    }

    [string]SerializeToLDL() {
        $ldlJson = [Newtonsoft.Json.JsonConvert]::SerializeObject($this, [Newtonsoft.Json.Formatting]::Indented)

        # post scrub to correct some of the elements
        #  1. remove { "ins" : "0100..." }, and replace with just "0100..." (i.e. just the instruction - they are not objects)
        $ldlJson = $ldlJson -replace '{\s+"ins": ("[0-9A-F]+")\s+}','$1'

        return $ldlJson
    }
}

<#
    .Description
    Simple static helper functions such as generating random numbers.
    9 Apr 2021
#>
class LDLHelper {
    static [int] RandomNumber([int]$min = 1, [int]$max = 10) {
        $max++
        $randomNumber = Get-Random -Minimum $min -Maximum $max
        return $randomNumber
    }

    static [Colour] RandomColour([Colour[]]$possibleColours) {
        $numColours = $possibleColours.Length - 1
        $randomColourIndex = [LDLHelper]::RandomNumber(0, $numColours)
        return $possibleColours[$randomColourIndex]
    }

    static [ColourQuantity[]] RandomColourQuantities([Colour[]]$possibleColours, [int]$minQuantityValue = 1, [int]$maxQuantityValue = 5, [int]$minElements = 2, [int]$maxElements = 5) {
        $randomNumberOfElements = [LDLHelper]::RandomNumber($minElements, $maxElements)
        $colourQuantities = [Collections.Generic.List[ColourQuantity]]::new()

        for($i = 0; $i -lt $randomNumberOfElements; $i++) {
            $randomQuantity = [LDLHelper]::RandomNumber($minQuantityValue, $maxQuantityValue)
            $randomColour = [LDLHelper]::RandomColour($possibleColours)
            $colourQuantity = [ColourQuantity]::new($randomColour, $randomQuantity)
            $colourQuantities.Add($colourQuantity)
        }

        return $colourQuantities
    }

    static [bool] RandomBool() {
        return [LDLHelper]::RandomNumber(0, 1)
    }

    static [Colour[]] RandomColours([Colour[]]$possibleColours, [byte]$minColours = 2, [byte]$maxColours = 5) {
        $randomNumberOfColours = [LDLHelper]::RandomNumber($minColours, $maxColours)
        $colours = [Collections.Generic.List[Colour]]::new()

        for($i = 0; $i -lt $randomNumberOfColours; $i++) {
            $randomColour = [LDLHelper]::RandomColour($possibleColours)
            $colours.Add($randomColour)
        }

        return $colours        
    }
}

# $ldlProgram = [LDLProgram]::new("Testingg")
# $infiniteLoop = $ldlProgram.AddInstructon([LoopLDLInstruction]::new(0))
# $infiniteLoop.AddInstruction([ClearEffectLDLInstruction]::new(10))
# $infiniteLoop.AddInstruction([SolidEffectLDLInstruction]::new(5, [Colour]::new(255, 0, 0)))
# $ldlJson = $ldlProgram.SerializeToLDL()
# Write-Host $ldlJson