var dogePrice;
Pebble.addEventListener("ready",
    function(e){
        console.log("js ready");
        Pebble.addEventListener("appmessage",
            function(){updateDoge()}
        );
        updateDoge(); 
    }
);

function httpGet(theUrl)
{
    var xmlHttp = null;

    xmlHttp = new XMLHttpRequest();
    xmlHttp.open( "GET", theUrl, false );
    xmlHttp.send( null );
    return xmlHttp.responseText;
}

function updateDoge(){
    response = JSON.parse(httpGet('http://pubapi.cryptsy.com/api.php?method=singlemarketdata&marketid=132'));
    if(response.success){
        dogePrice = response.return.markets.DOGE.lasttradeprice;
        console.log(dogePrice);
    }
    else{
        dogePrice = 'Error!';
        console.log("error:"+response.success);
    }
    var transactionId = Pebble.sendAppMessage( { "0": dogePrice },
      function(e) {
        console.log("Successfully delivered message with transactionId="
          + e.data.transactionId);
      },
      function(e) {
        console.log("Unable to deliver message with transactionId="
          + e.data.transactionId
          + " Error is: " + e.error.message);
      }
    );
}