<#
    .Description
    Simple LDL server UDP discovery service.  This service broadcasts
    a UDP packet on port 8888 with a simple message containing the
    handshake (LDL-HOLA?).  Then, replies are listened for for a short
    while.  Those replies are then checked to determine whether they
    contained the expected reponse for a handshake.  The expected
    response consists of a JSON reply containing the LDL server version number
    as well as other information.
    Kevin White
    30 Mar 2021

    .Example
    # The following is the basic usage for the discovery service
    [LdlServerDiscover]$discoverService = New-Object LdlServerDiscover -ArgumentList "8888"
    $discoveredServers = $discoverService.DiscoverServers()

    $discoveredServers | ForEach-Object {
     [LdlServerInfo]$ldlServerInfo = $_
        #   ...useful and exciting code...
    }

    # instead of $discoveredServers = $discoverService.DiscoverServers()
    # the following can be used in order to output some useful console messages
    $discoveredServers = [LdlServerDiscover]::DiscoverServersVerbose($discoverService)
#>




<#
    .Description
    Stores the information relevant to about an LDL server including:
        - ip address
        - ldl server version
    Kevin White
    22 Mar 2021
#>
class LdlServerInfo {
    [string]$ip
    [string]$server

    <#
        .Description
        Constructs a new LdlServerInfo by transforming a
        plain object to a strongly=typed LdlServerInfo.
        Kevin White
        22 Mar 21
        .Parameter      hashTable   [HashTable]The plain object that will be transformed to a LdlServerInfo instance
        .Parameter      ip          [string]The IP address of the LDL server
    #>    
    static [LdlServerInfo]FromHashTable($hashTable, $ip) {
        if($null -eq $hashTable `
            -or $null -eq $hashTable.server `
            -or $null -eq $ip) {
            return $null
        }

        $serverInfo = New-Object LdlServerInfo
        $serverInfo.server = $hashTable.server
        $serverInfo.ip = $ip

        return $serverInfo
    }
}

<#
    .Description
    Discovers any LDL servers that might be running of the local
    network by broadcasting a UDP packet on a specific port.
    Kevin White
    22 Mar 2021
#>
class LdlServerDiscover {
    [string]$port = "8888"
    hidden [string]$handshake = "LDL-HOLA?"
    hidden $handshakeBytes
    hidden $maxDiscoverSeconds = 10

    hidden $asciiEncoding

    <#
        .Description
        Constructor initialises the discovering port number.
        Kevin White
        22 Mar 21
        .Parameter      port    [int]The port number on which to broadcast the discovery packet.
    #> 
    LdlServerDiscover($port = 8888) {
        $this.port = $port

        $this.asciiEncoding = new-object system.text.asciiencoding
        $this.handshakeBytes = $this.asciiEncoding.GetBytes($this.handshake)
    }

    <#
        .Description
        Checks the responses received from broadcasting the UDP discovery
        message to see if any have responded with the expected handshake response.
        Kevin White
        30 Mar 21
        .Outputs    [array]Collecton of discovered servers as LdlServerInfo instances.
    #>    
    hidden [array]ProcessUdpReplies($ldlServersPotentiallyFound) {
        $discoveredLdlServers = New-Object 'Collections.Generic.List[LdlServerInfo]'

        for($keyCounter = 0; $keyCounter -lt $ldlServersPotentiallyFound.Count; $keyCounter++) {
            $ip = $ldlServersPotentiallyFound.Keys[$keycounter]
            $reply = $ldlServersPotentiallyFound.Values[$keyCounter]
            $replyObj = $reply | ConvertFrom-Json
            
            if($null -ne $replyObj) {
                # a valid response is one that is in the expected JSON format
                # this reply includes the version of the LDL server
                [LdlServerInfo]$serverInfo = [LdlServerInfo]::FromHashTable($replyObj, $ip)
                if($null -ne $serverInfo) {
                    $discoveredLdlServers.Add($serverInfo)
                }
            }                
        }

        return $discoveredLdlServers
    }

    <#
        .Description
        Discovers LDL servers running on the local network by broadcasting
        a UDP handshake message and checking received replies for expected
        LDL handshake response.
        Kevin White
        22 Mar 21
        .Outputs    [array]Collecton of discovered servers as LdlServerInfo instances.
    #>
    [array]DiscoverServers() {
        $receiveEndpoint = New-Object system.net.ipendpoint([system.net.ipaddress]::Any,$this.port)
        $broadcastEndPoint = New-Object system.net.ipendpoint([system.net.ipaddress]::Broadcast,$this.port)
        
        # $asciiEncoding = new-object system.text.asciiencoding
        $discoveredLdlServers = New-Object 'Collections.Generic.List[LdlServerInfo]'

        $udpClient = new-Object system.Net.Sockets.Udpclient
        try {
            $udpClient.EnableBroadcast = $true
            $udpClient.Client.Bind($receiveEndpoint)
            $udpClient.Client.ReceiveTimeout = 2000

            # broadcast the handshake message
            [void]$udpClient.Send($this.handshakeBytes,$this.handshakeBytes.length, $broadcastEndPoint)

            # listen for responses for 10 seconds max
            $startTime = $(get-date)
            $elapsed = 0
            $receivebytes = $null
            $countFailures = 0
            $ldlServersPotentiallyFound = @{}
            while($elapsed -le $this.maxDiscoverSeconds -and $countFailures -lt 2) {
                Try {
                    $receivebytes = $udpClient.Receive([ref]$receiveEndpoint)
                    $from = $receiveEndpoint.Address.ToString()

                    [string]$returndata = $this.asciiEncoding.GetString($receivebytes)

                    if($null -ne $returndata -and $returndata -ne $this.handshake) {
                        # we get one!
                        $ldlServersPotentiallyFound.Add($from, $returndata)
                    }
                } Catch {
                    $countFailures++
                }
                $currentTime = $(get-date)
                $elapsedTime = $currentTime - $startTime
                $elapsed = $elapsedTime.TotalSeconds
            }

            # see what we got - any LDL servers?
            $discoveredLdlServers = $this.ProcessUdpReplies($ldlServersPotentiallyFound)
        }
        finally {
            $udpClient.Close()
        }

        return $discoveredLdlServers
    }

    <#
        .Description
        Attempts to discover the LDL servers on the local network and additionally
        outputs some messages to the console to show the progress of the discovering process.
        Kevin White
        30 Mar 2021
    #>    
    static [array]DiscoverServersVerbose([LdlServerDiscover]$discoverService) {
        Write-Host "Discovering LDL servers on the local network via UDP..." -ForegroundColor Green
        $servers = $discoverService.DiscoverServers();
    
        if($null -eq $servers -or $servers.Count -eq 0) {
            Write-Host "...could not find any servers.  Are they connected to the local network?" -ForegroundColor Red
        } else {
            $servers | ForEach-Object {
                $serverIp = $_.ip
                $reply = $_ | ConvertTo-Json
                Write-Host "...found server @ $serverIp with reply $reply" -ForegroundColor Green
            }
        }
    
        return $servers        
    }
}