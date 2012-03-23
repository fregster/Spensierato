<!DOCTYPE table PUBLIC "-//W3C//DTD HTML 4.01 Transitional//EN" "http://www.w3.org/TR/html4/loose.dtd">
<html>
<title>Error Viewing Page</title>
<body>
<?php 


//Get list of error files
$error_files = glob(ERROR_LOG_DIR.DIRECTORY_SEPARATOR.'*');

//Sort the error files by modified date (last seen)
usort($error_files, create_function('$a,$b', 'return filemtime($a) - filemtime($b);'));
$error_files = array_reverse($error_files);
$count = count($error_files);

echo '<p>There are currently <strong>' . $count . '</strong> errors been tracked</p>';

$i = 0;
$limit = 10;
?>
The last 10 seen errors
<table>
  <tr>
    <th>Error Code</th>
    <th>Created</th>
    <th>Last Seen</th>
  </tr>
<?php 
	while ($i < $limit && ($i+1 <= $count)){
		echo '<tr><td><a href="?id='.basename($error_files[$i]).'#error">'. basename($error_files[$i]) .'</a></td><td>'.date('r', filectime($error_files[$i])).'</td><td>'.date('r', filemtime($error_files[$i])).'</td></tr>';
		$i++;
	}
?>
</table>

<?php 
if(get::variable('id', 'isset') && common::is_valid_hash(get::variable('id')) && file_exists(ERROR_LOG_DIR.DIRECTORY_SEPARATOR.get::variable('id'))){
	
	echo '<div id="error_details"><a name="error"/><h2>Error: '.get::variable('id').'</h2><pre>';
	@ini_set('html_errors', 'on');
	@ini_set('xdebug.var_display_max_depth', 8);
	var_dump(unserialize(file_get_contents(ERROR_LOG_DIR.DIRECTORY_SEPARATOR.get::variable('id'))));
	echo'</pre></div>';
}


?>
</body>
</html>