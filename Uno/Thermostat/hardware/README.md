# Hardware

## Parts list
* [Arduino Uno R3](https://www.adafruit.com/product/50)
* [Platinum RTD Sensor - PT100 - 3 Wire 1 meter long](https://www.adafruit.com/product/3290)
* [Adafruit PT100 RTD Temperature Sensor Amplifier - MAX31865](https://www.adafruit.com/product/3328)
* [2-Channel 5V Relay Module](https://www.sainsmart.com/products/2-channel-5v-relay-module)
* [Adafruit 0.56" 4-Digit 7-Segment Display w/I2C Backpack - Red](https://www.adafruit.com/product/878)

Not enough memory to use?
* [MicroSD card breakout board+](https://www.adafruit.com/product/254)

---

## Assembly


<table>
  <tr>
    <td><b><a "https://www.adafruit.com/product/3328">MAX31865</a></b></td>
    <td><b><a "https://www.adafruit.com/product/50">Arduino Uno R3</a></b></td>
  </tr>
  <tr>  <td>VIN</td>  <td>5V</td>   </tr>
  <tr>  <td>GND</td>  <td>GND</td>  </tr>
  <tr>  <td>3V3</td>  <td></td>     </tr>
  <tr>  <td>CLK</td>  <td>~13</td>  </tr>
  <tr>  <td>SDO</td>  <td>~12</td>  </tr>
  <tr>  <td>SDI</td>  <td>~11</td>  </tr>
  <tr>  <td>CS</td>   <td>~10</td>  </tr>
  <tr>  <td>RDY</td>  <td></td>     </tr>
</table>
<table>
  <tr>
    <td><b><a "https://www.adafruit.com/product/3328">MAX31865</a></b></td>
    <td><b><a "https://www.adafruit.com/product/3290">RTD Sensor</a></b></td>
  </tr>
  <tr>  <td>F+</td>    <td>red</td>   </tr>
  <tr>  <td>RTO+</td>  <td>red</td>  </tr>
  <tr>  <td>RTO-</td>  <td>blue</td>     </tr>
  <tr>  <td>F-</td>    <td></td>  </tr>
</table>

Jumper/solder the 2/3 Wire pads

2 4 3 pad:  Cut the 2/4 jumper, solder/jumper the 4/3 pads

---

<table>
  <tr>
    <td><b><a "https://www.sainsmart.com/products/2-channel-5v-relay-module">Relay Module</a></b></td>
    <td><b><a "https://www.adafruit.com/product/50">Arduino Uno R3</a></b></td>
  </tr>
  <tr>  <td>GND</td>  <td>GND</td>  </tr>
  <tr>  <td>IN1</td>  <td>8</td>    </tr>
  <tr>  <td>IN2</td>  <td>~9</td>   </tr>
  <tr>  <td>VCC</td>  <td>5V</td>   </tr>
</table>
<table>
  <tr>
    <td><b><a "https://www.sainsmart.com/products/2-channel-5v-relay-module">Relay Module</a></b></td>
    <td><b>Water Heater</b></td>
  </tr>
  <tr>  <td>K1 open</td>    <td>Red</td>    </tr>
  <tr>  <td>K1 center</td>  <td>Black</td>  </tr>
  <tr>  <td>K1 closed</td>  <td></td>       </tr>
  <tr>  <td>K2 open</td>    <td></td>       </tr>
  <tr>  <td>K2 center</td>  <td></td>       </tr>
  <tr>  <td>K2 closed</td>  <td></td>       </tr>
</table>

Jumper JD-VCC and VCC

---

<table>
  <tr>
    <td><b><a "https://www.adafruit.com/product/878">Display</a></b></td>
    <td><b><a "https://www.adafruit.com/product/50">Arduino Uno R3</a></b></td>
  </tr>
  <tr>  <td>+</td>  <td>5V</td>   </tr>
  <tr>  <td>-</td>  <td>GND</td>  </tr>
  <tr>  <td>D</td>  <td>A4</td>   </tr>
  <tr>  <td>C</td>  <td>A5</td>   </tr>
</table>
