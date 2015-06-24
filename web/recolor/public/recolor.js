/// Выводит в консоль все проперти переменной
/// eval( printAllKeyValues( 'var_name' ) )
function printAllKeyValues(name) {
  return "for (var key in " + name + ") console.log('" + name + ".' + key + ' = ' + " + name + "[key])"
}

function recolor() {
  console.log('welcome to Recolor')

  if (!!window.EventSource) {
    eval( printAllKeyValues( 'EventSource' ) )

    var source = new EventSource('http://127.0.0.1:9090/recolor/thing')
    eval( printAllKeyValues( 'source' ) )

    source.addEventListener('message', function(e) {
      console.log('STATE:' + source.readyState + ' EVENT: ' + e.event + ' MESSAGE:' + e.data)
    }, false)

    source.addEventListener('open', function(e) {
      console.log('STATE:' + source.readyState + ' EVENT: ' + e.event + ' OPEN: ' + e.data)
    }, false)

    source.addEventListener('error', function(e) {
      console.log('STATE:' + source.readyState + ' EVENT: ' + e.event + ' ERROR: ' + e.data)
      source.close()
    }, false)

  } else {
    console.log('Error: can\'t stream recolor-tool log')
  }
}
