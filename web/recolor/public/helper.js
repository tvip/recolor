/// Выводит в консоль все проперти переменной
/// eval( printAllKeyValues( 'var_name' ) )
function printAllKeyValues(name) {
  return "for (var key in " + name + ") console.log('" + name + ".' + key + ' = ' + " + name + "[key])"
}