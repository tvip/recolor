function recolor() {
  console.log('welcome to Recolor')

  if (!!window.EventSource) {
    var source = new EventSource('http://127.0.0.1:9090/recolor/thing')
/*
    source.addEventListener('message', function(e) {
      console.log('RECOLOR_TOOL: LOG:' + e.data);
    }, false)
*/
    source.onmessage = function(e) {
      console.log('RECOLOR_TOOL: LOG:' + e.data);
    }

    source.addEventListener('open', function(e) {
      console.log('RECOLOR_TOOL: connection established: ' + e.message)
    }, false)

    source.addEventListener('error', function(e) {
      if (e.readyState == EventSource.CLOSED) {
        console.log('RECOLOR_TOOL: connection closed: ' + e.message)
      }
      else {
        console.log('RECOLOR_TOOL: connection error: ' + e.message)
      }
    }, false)

  } else {
    console.log('Error: can\'t stream recolor-tool log')
  }
}