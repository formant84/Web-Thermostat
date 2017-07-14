
//Handle communication via WebSocket
var ws = new WebSocket("ws://86.7.33.117:81");
ws.onopen = function () 
{
    ws.send("Socket Open!");    
};
           
ws.onerror = function (error) {
ws.log('WebSocket Error ', error);
};
           		
ws.onmessage = function (evt)
    { 
    var received_msg = evt.data;
    if (received_msg.startsWith("Temp"))
        {
            var tempValue = received_msg.substr(4);
            console.log("Received temp message: " + tempValue)
            var tempInt = parseInt(tempValue);    
            tempslider.setValue(tempInt);
        }
    if (received_msg.startsWith("Time"))
        {
            var timeValue = received_msg.substr(4);
            console.log("Received time message: " + timeValue)    
            timeslider.setValue(timeValue);
        }
    
    if (received_msg.startsWith("R"))
        {
            var realTempVal = received_msg.substr(1);   
            document.getElementById("realTemp").innerHTML=(realTempVal+ '&#8451;');
            console.log("Received temperature reading "+realTempVal+"C");
        }
    
    if (received_msg.startsWith("H"))
        {
            var humidVal = received_msg.substr(1);   
            document.getElementById("humidity").innerHTML=(humidVal+ '%');
            console.log("Recieved Humidity reading "+humidVal+"%");
        }
    
    if (received_msg.startsWith("P"))
        {
            var pressureVal = received_msg.substr(1);   
            document.getElementById("pressure").innerHTML=(pressureVal+ 'mB');
            console.log("Recieved pressure reading "+pressureVal+"mB");
        }
           
    };



/*-----------------------------Temp slider JS-------------------------*/
$( document ).ready(function() {



 // this is core functionality to generate the numbers
$.fn.roundSlider.prototype.defaults.create = function() {
  var o = this.options, tickInterval = 2;
  for (var i = o.min; i <= o.max; i += tickInterval) {
    var angle = this._valueToAngle(i);
    var numberTag = this._addSeperator(angle, "rs-custom");
    var number = numberTag.children();
    number.clone().css({
      "width": o.width + this._border(),
      "margin-top": this._border(true) / -2
    }).appendTo(numberTag);
    number.removeClass().addClass("rs-number").html(i).rsRotate(-angle);
  }
}

/// ### ---- ### sample level workaround ### ---- ### ///
var fn1 = $.fn.roundSlider.prototype._setProperties;
$.fn.roundSlider.prototype._setProperties = function () {
  fn1.apply(this);
  var o = this.options, r = o.radius, d = r * 2,
      r1 = r - (o.width / 2) - this._border(true),
      svgNS = "http://www.w3.org/2000/svg";
  this._circum = Math.PI * (r1 * 2);

  var $svg = $(document.createElementNS(svgNS, "svg"));
  $svg.attr({ "height": d, "width": d });

  this.$circle = $(document.createElementNS(svgNS, 'circle')).attr({
    "fill": "transparent", "class": "rs-transition", "cx": r, "cy": r, "r": r1,
    "stroke-width": o.width, "stroke-dasharray": this._circum
  }).appendTo($svg);

  this.$svg_box = $(document.createElement("div")).addClass("rs-transition rs-svg").append($svg).css({
    "height": d, "width": d, "transform-origin": "50% 50%",
    "transform": "rotate(" + (o.startAngle + 180) + "deg)"
  }).appendTo(this.innerContainer);
}

var fn2 = $.fn.roundSlider.prototype._changeSliderValue;
$.fn.roundSlider.prototype._changeSliderValue = function (val, deg) {
  fn2.apply(this, arguments);
  deg = deg - this.options.startAngle;

  if (this._rangeSlider) {
    this.$svg_box.rsRotate(this._handle1.angle + 180);
    deg = this._handle2.angle - this._handle1.angle;
  }
  var pct = (1 - (deg / 360)) * this._circum;
  this.$circle.css({ strokeDashoffset: pct });
}
/// ### ---- ### --------------------- ### ---- ### ///

window.changeTooltip = function(e){
    return e.value + "&deg;C";
}
    
window.tempValueChange = function(e) {
    console.log("Temp change to "+(e.value)+"C");
    ws.send("SetTemp"+(e.value)); 
}
window.timeValueChange = function(e) {
    console.log("time change to "+(e.value));
    ws.send("SetTime"+(e.value));
    
}    


$("#tempslider").roundSlider({
    sliderType: "default",
    editableTooltip: false,
    min: 10,
    max: 36,
    radius: 120,
    width: 24,
    value: 25,
    handleSize: 0,
    handleShape: "dot",
    circleShape: "pie",
    handleSize: "+6",
    startAngle: 315,
    tooltipFormat: "changeTooltip",
    change: "tempValueChange",
    
});

});

/* ------------Time Range Slider --------------*/



$( document ).ready(function() {
    

// this is core functionality to generate the numbers
$.fn.roundSlider.prototype.defaults.create = function() {
  var o = this.options, tickInterval = 1;
  for (var i = o.min; i <= o.max-1; i += tickInterval) {
    var angle = this._valueToAngle(i);
    var numberTag = this._addSeperator(angle, "rs-custom");
    var number = numberTag.children();
    number.clone().css({
      "width": o.width + this._border(),
      "margin-top": this._border(true) / -2
    }).appendTo(numberTag);
    number.removeClass().addClass("rs-number").html(i).rsRotate(-angle);
  }
}


 window.timeTooltip = function (e) {
    var parts = parseFloat(e.value).toFixed(2).split(".");
    var sec = parts[1] == "00" ? "00" : 60*(parts[1]/100);
    var value = parts[0] + ":" + sec;
    return (value);
  }
var _getTooltipPos = $.fn.roundSlider.prototype._getTooltipPos;
$.fn.roundSlider.prototype._getTooltipPos = function () {
	if(!this.tooltip.is(":visible"))
		$("body").append(this.tooltip);
  var pos = _getTooltipPos.call(this);
  this.container.append(this.tooltip);
  return pos;
}

$("#timeslider").roundSlider({
  sliderType: "range",
  startAngle: 90,
  editableTooltip: false,
  handleShape: "dot",
  handleSize: "+6",
  radius: 120,
  width: 24,
  min: 0,
  max: 24,
  step: 0.25,
  startAngle: 90,
  value: "13",
  change: "timeValueChange",
  tooltipFormat:"timeTooltip",
 
});
});







/*

$("#slider-range").slider({
    range: true,
    min: 0,
    max: 1440,
    step: 15,
    values: [600, 720],
    slide: function (e, ui) {
        var hours1 = Math.floor(ui.values[0] / 60);
        var minutes1 = ui.values[0] - (hours1 * 60);

        if (hours1.length == 1) hours1 = '0' + hours1;
        if (minutes1.length == 1) minutes1 = '0' + minutes1;
        if (minutes1 == 0) minutes1 = '00';
        if (hours1 >= 12) {
            if (hours1 == 12) {
                hours1 = hours1;
                minutes1 = minutes1 + " PM";
            } else {
                hours1 = hours1 - 12;
                minutes1 = minutes1 + " PM";
            }
        } else {
            hours1 = hours1;
            minutes1 = minutes1 + " AM";
        }
        if (hours1 == 0) {
            hours1 = 12;
            minutes1 = minutes1;
        }



        $('.slider-time').html(hours1 + ':' + minutes1);

        var hours2 = Math.floor(ui.values[1] / 60);
        var minutes2 = ui.values[1] - (hours2 * 60);

        if (hours2.length == 1) hours2 = '0' + hours2;
        if (minutes2.length == 1) minutes2 = '0' + minutes2;
        if (minutes2 == 0) minutes2 = '00';
        if (hours2 >= 12) {
            if (hours2 == 12) {
                hours2 = hours2;
                minutes2 = minutes2 + " PM";
            } else if (hours2 == 24) {
                hours2 = 11;
                minutes2 = "59 PM";
            } else {
                hours2 = hours2 - 12;
                minutes2 = minutes2 + " PM";
            }
        } else {
            hours2 = hours2;
            minutes2 = minutes2 + " AM";
        }

        $('.slider-time2').html(hours2 + ':' + minutes2);
    }
});
*/