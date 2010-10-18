<?php 

if($_SERVER['REQUEST_METHOD'] == 'POST')
{
	
	foreach($_POST as $key => $value)
	{
		echo(post::variable($key));
		
		Database::singleton()->sql_update_or_insert('settings', $where_fields = array(), $where_values = array(), $fields = array(), $values = array(), $destroy_cache = false, $alternet_id_col = false);
		
	}
	
	die();
	
}

?>

<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
	<head>
		<title>Site Details</title>
	</head>
	<body>
		<h2>Site Details</h2>
		<form action="" method="post">

<?php

$settings = Settings::singleton()->get_all_settings(true);

foreach ($settings as $key => $setting_group)
{
	echo('<table id="setting_group_'.$key.'">');
	
	foreach($setting_group as $setting)
	{
		?>
		<tr>
				<td><?php echo($setting['name']); ?></td><td><input type="text" value="<?php echo($setting['value']); ?>" name="<?php echo($setting['name']); ?>" id="<?php echo($setting['name']); ?>" /></td>
		</tr>
		<?php 
	}
	
	echo('</table>');
}

?>
		<input type="submit" value="Save"/>
		</form>
	</body>
</html>