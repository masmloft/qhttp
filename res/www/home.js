
function ajax_onFail( jqxhr, textStatus, error ) {
     var err = textStatus + ', ' + error;
     console.log( "Request Failed: " + err);
}

function timeToString(inDate) {
    let dateStr = null;
    let timeStr = null;
    if (inDate) {
        const zero = inDate.getMinutes() < 10 ? '0' : '';
        const sZero = inDate.getSeconds() < 10 ? '0' : '';
        dateStr = inDate.toLocaleDateString();
        timeStr = inDate.getHours() + ':' + zero + inDate.getMinutes() + ':' + sZero + inDate.getSeconds();
    }
    return { date: dateStr, time: timeStr, all: inDate ? dateStr + ' ' + timeStr : null };
}

function ajax_onState( json ) {
    const dts = timeToString(new Date(json.time));
    $("#DateView").text(dts.date);
    $("#TimeView").text(dts.time);
    $("#TemperatureView").text(json.temperature);
}


$(document).ready(function () {
      setInterval(function() { $.getJSON("/api/state").done(ajax_onState).fail(ajax_onFail); }, 1000);
});
