const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>

<head>
</head>
<html>
<style>

mark::before, mark::after, del::before, del::after, ins::before, ins::after, s::before, s::after {
  clip-path: inset(100%);
  clip: rect(1px, 1px, 1px, 1px);
  height: 1px;
  width: 1px;
  overflow: hidden;
  position: absolute;
  white-space: nowrap;
}
    body {
        font-family: Arial, Helvetica, sans-serif;
        color: black;
    }
    a:link{color:firebrick;  text-decoration: none;}
    a:hover{color: darkred;  text-decoration: none;}

    * {
        box-sizing: border-box;
    }

    h3 {
        margin-bottom: 0px;
    }

    .detail {
        margin-top: 0px;
        font-size: .8em;
        margin-bottom: 0px;
        margin-left: 12px;
        color: gray;
    }

    .valuetitle {
        margin-bottom: .5em;
    }

    .currentvalues {
        float: right;
        text-align: right;
        color: grey;
    }

    .form-inline {
        display: flex;
        flex-flow: column;
        align-items: left;
    }

    .form-inline label {
        margin: 5px 10px 5px 0;
        display: block;
        font-size: 1em;
    }

    .form-inline input {
        vertical-align: middle;
        margin: 5px 10px 5px 0;
        padding: 12px 20px;
        background-color: #fff;
        border: 1px solid #ccc;
        border-radius: 10px;
        width: 100%;
    }

    input[type='radio'] {
        width: auto;
        vertical-align: baseline;
        margin: 15px 0px 5px 0;

    }

    input[type='radio']+label {
        display: inline;
        width: auto;
        margin: 15px 10px 5px .01em;

    }

    .form-inline button {
        padding: 10px 20px;
        background-color: FireBrick;
        border: 1px solid #ddd;
        color: white;
        cursor: pointer;

    }

    .form-inline button:hover {
        background-color: darkred;
    }

    @media (max-width: 900px) {
        .form-inline input {
            margin: 10px 0;
        }

        .form-inline {
            flex-direction: column;
            align-items: stretch;
            width: 100%;
        }

        .row {
            flex-direction: column;
        }

        .col-25 {
            margin-bottom: 20px;
        }
    }

    .row {
        display: -ms-flexbox;
        /* IE10 */
        display: flex;
        -ms-flex-wrap: wrap;
        /* IE10 */
        flex-wrap: wrap;
        margin: 0 -16px;
    }

    .row2 {
        display: -ms-flexbox;
        /* IE10 */
        display: flex;
        -ms-flex-wrap: nowrap;
        /* IE10 */
        flex-wrap: nowrap;
        margin: 0 -16px;
    }

    .col-25 {
        -ms-flex: 25%;
        /* IE10 */
        flex: 25%;
    }

    .col-50 {
        -ms-flex: 50%;
        /* IE10 */
        flex: 50%;
    }

    .col-75 {
        -ms-flex: 75%;
        /* IE10 */
        flex: 75%;
    }

    .col-100 {
        -ms-flex: 100%;
        /* IE10 */
        flex: 100%;
    }

    .col-25,
    .col-50,
    .col-75,
    .col-100 {
        padding: 0 16px;
    }

    .container {
        background-color: #f2f2f2;
        padding: 5px 20px 15px 20px;
        border: 1px solid lightgrey;
        border-radius: 3px;
    }

    #content {
        display: "none";
    }
</style>
<title>Config Page</title>

<body>

    <div class="row">
        <div class="col-75">
            <div class="container">
                <form action='/' method='POST' class="form-inline"
                    onsubmit='setTimeout(function () { window.location.reload(); }, 5)'>
                    <div class="row">
                        <div class="col-100">
                            <h1 style="text-align:center;">FlashBang</h1>
                            <h2 style="text-align:center;">An OSC Trigger Device</h2>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-100">
                            <h3>Updatable Values</h3>
                        </div>
                    </div><br>
                    <div class="row">
                        <div class="col-25"><label for="ID">Device Identifier<span class="detail">Spaces will be
                                    converted to
                                    underscores.</span></label>
                            <input type='text' name='ID' placeholder='Device Identifier' value="@@id@@">
                        </div>
                        <div class="col-25">
                            <label for="IP">Send to IP address</label>
                            <input type='text' name='IP' placeholder='IP to send to' value="@@ip@@">
                        </div>
                        <div class="col-25">
                            <label for="PORT">Sending on port</label>
                            <input type='number' name='PORT' placeholder='Port to send on' min="0" max="65535"
                                value="@@port@@">
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-25">
                            <label for="DEBOUNCE">BUTTON DEBOUNCE TIME</label>
                            <p class="detail">Time in millis from last button press till another button press is
                                allowed. Min: 50</p>
                            <input type='number' name='DEBOUNCE' placeholder='in millis' min="50" value="@@debounce@@">
                        </div>
                        <div class="col-25">
                            <label for="RAPID">RAPID FIRE</label>
                            <p class="detail">Pressing and holding button will continue to advance cues based on
                                Button Debounce Time</p>
                            <input type="radio" id="No" value="No" name="RAPID">
                            <label for="NO"
                                title="Press and holding will only allow adv of one cue.  Need to release and push button again for next cue">No</label>
                            <input type="radio" id="Yes" value="Yes" name="RAPID">
                            <label for="YES"
                                title="As long as button is pressed, cues will advance at the rate of the Button Debounce Time">Yes</label>
                            <!--  <input type="radio" id="NoChange" value="" name="RAPID" checked="checked">
                            <label for="NoChange" style="font-size:70%">Leave unchanged</label>-->
                        </div>
                        <div class="col-25">
                            <label for="RESETTIME">RESET TIME</label>
                            <p class="detail">
                            <p class="detail">Time in millis from last button press till reseting to cue start
                                number</p>
                            <input type='number' name='RESETTIME' placeholder='in millis' min="0" value="@@resettime@@">
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-100">
                            <h3>Entire command will be combined together internally.</h3>
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-25">
                            <label for="CMDBEGIN">COMMAND Begining</label>
                            <p class="detail">example:<mark><u>/cue/</u></mark>2/start</p>
                            <input type='text' name='CMDBEGIN' placeholder='Command begining' value="@@cmdbegin@@">
                        </div>
                        <div class="col-25">
                            <label for="CUE">CUE Start Number</label>
                            <p class="detail">/cue/<mark><u>2</u></mark>/start</p>
                            <input type='number' name='CUE' placeholder='Starting cue number' min="0" value="@@cue@@">
                        </div>
                        <div class="col-25">
                            <label for="CMDEND">COMMAND End</label>
                            <p class="detail">/cue/2<mark><u>/start</u></mark></p>
                            <input type='text' name='CMDEND' placeholder='End of Command' value="@@cmdend@@">
                        </div>
                    </div>
                    <div class="row">
                        <div class="col-25">
                            <label for="NUMOFCUE">NUMBER of Cues</label>
                            <input type='number' name='NUMOFCUE' placeholder='Number of unique cues' min="1"
                                value="@@numofcue@@">
                        </div>
                        <div class="col-25">
                            <label for="FLASHLENGTH">TIME LED is ON</label>
                            <p class="detail">Time in millis</p>
                            <input type='number' name='FLASHLENGTH' placeholder='in millis' min="0"
                                value="@@flashlength@@">
                        </div>
                        <div class="col-25">
                            <label for="FADETIME">FADE OUT Time</label>
                            <p class="detail">Time in millis after on time</p>
                            <input type='number' name='FADETIME' placeholder='in millis' min="0" value="@@fadetime@@">
                        </div>
                    </div>
                    <button type='submit' name='SUBMIT'>Save Changes</button>
                </form>

            </div>
        </div>
        <div class="col-25">
            <div class="container">
                <div class="row2">
                    <div class="col-100">
                        <h2 style="text-align:center">Current Values</h2>
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Last Command:<p class="detail">Updated every 1 sec.</p>
                    </div>
                    <div class="col-50 currentvalues" id="LastCommand">
                        Loading...
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        <a href="/ap">SSID:</a>
                    </div>
                    <div class="col-50 currentvalues">
                        @@ssid@@
                    </div>
                </div>
                <div class="row2" onclick="hidepw()">
                    <div class="col-50 valuetitle">
                        Password:
                    </div>
                    <div class="col-50 currentvalues" id="content">
                        <span class="detail">Click to view</span>
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        MAC:
                    </div>
                    <div class="col-50 currentvalues">
                        @@mac@@
                    </div>
                </div>
                <br>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Device ID:
                    </div>
                    <div class="col-50 currentvalues">
                        @@id@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Sending to IP:
                    </div>
                    <div class="col-50 currentvalues">
                        @@ip@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Sending to Port:
                    </div>
                    <div class="col-50 currentvalues">
                        @@port@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Debouce time:
                    </div>
                    <div class="col-50 currentvalues">
                        @@debounce@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Rapid fire:
                    </div>
                    <div class="col-50 currentvalues">
                        @@rapid@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Reset time:
                    </div>
                    <div class="col-50 currentvalues">
                        @@resettime@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Command:
                    </div>
                    <div class="col-50 currentvalues">
                        @@fullcmd@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Command Begin:
                    </div>
                    <div class="col-50 currentvalues">
                        @@cmdbegin@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Cue Start Number:
                    </div>
                    <div class="col-50 currentvalues">
                        @@cue@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Command End:
                    </div>
                    <div class="col-50 currentvalues">
                        @@cmdend@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Number of Cues:
                    </div>
                    <div class="col-50 currentvalues">
                        @@numofcue@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        Time LED is on:
                    </div>
                    <div class="col-50 currentvalues">
                        @@flashlength@@
                    </div>
                </div>
                <div class="row2">
                    <div class="col-50 valuetitle">
                        LED fade time:
                    </div>
                    <div class="col-50 currentvalues">
                        @@fadetime@@
                    </div>
                </div>
                <div class="row">
                    <div class="col-100">
                        <form action='/' method='POST' class="form-inline">
                            <input type='hidden' name='SENDUDP' value='1'>
                            <button type='submit' name='SENDUDP'>Send Test Packet</button>
                        </form>
                    </div>
                </div>
            </div>
        </div>
    </div>
    <script src="https://code.jquery.com/jquery-3.5.1.min.js"></script>
    <script>
        hidepw();
        function hidepw() {
            var x = document.getElementById("content");
            if (x.innerHTML === "<span class=\"detail\">Click to view</span>") {
                x.innerHTML = "@@pass@@";
            } else {
                x.innerHTML = "<span class=\"detail\">Click to view</span>";
            }
        }
        setInterval(function () {
            getData();
        }, 1000);
        function getData() {
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function () {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("LastCommand").innerHTML =
                        this.responseText;
                }
            };
            xhttp.open("GET", "info", true);
            xhttp.send();
        }
        $("input[name=RAPID][value=@@rapid@@]").prop('checked', true);
    </script>
</body>

</html>
)=====";
