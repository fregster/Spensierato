<?php
if (!user::Singleton()->is_logged_in() || !user::Singleton()->is_admin()) common::bounce_to_referer();
$updated = false;
if($_SERVER['REQUEST_METHOD'] == 'POST')
{
	foreach($_POST as $key => $value)
	{		
		$new_value = post::variable($key);
		
		if(Settings::singleton()->update_database_setting($key, $new_value)){
			$updated = true;
		}
	}
	if($updated){
		persistantcache::Singleton()->cycle_namespace('settings');
		Settings::Singleton()->get_database_settings(true);
		die('
			<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
			<html xmlns="http://www.w3.org/1999/xhtml">
			<head>
			<meta http-equiv="Content-Type" content="text/html; charset=iso-8859-1" />
			<title>Saved</title>
			<link rel="stylesheet" type="text/css" href="style.css" />
			<script language=javascript>window.open("'.common::document_root().'/tools/site_details", "_self", "")</script>
			</head>
			<body>
			<p>
			Your updates have been saved.<br />
			You should be redirected back the page the settings page in a few seconds<br/>
			If not click <a href="'.common::document_root().'/tools/site_details" target="_self">here</a>
			</p>

			</body>
			</html>');
	}
}

//Ensure the template class is loaded to ensure all the settings relating to the skin are shown
template::singleton();

?><!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Transitional//EN"
   "http://www.w3.org/TR/xhtml1/DTD/xhtml1-transitional.dtd">
<html>
	<head>
		<title>Site Details</title>
		<link href="<?php echo common::document_root().'/css/v/'.settings::singleton()->get_setting('skin').'/'.settings::singleton()->get_setting('template_version') ?>" type="text/css" rel="stylesheet">
	</head>
	<body style="background-color:white;background-image:none;">
<?php 
if($updated){
	echo '<div>Your settings have been updated</div>';
}
?>
		<h2>Site Details</h2>
		<form action="" method="post">
		<input type="submit" value="Save"/><br/><br/>
<?php

$settings = Settings::singleton()->get_all_settings(true, true);

foreach ($settings as $key => $setting_group)
{
	echo '<div style="text-transform:capitalize; margin-top:20px;">'.strtolower(str_replace('_', ' ', $key)).'</div>';
	echo('<table id="setting_group_'.$key.'" style="background-color:#EEE; border:1px; margin-bottom:10px; padding:3px;">');
	
	foreach($setting_group as $setting)
	{
		?>
		<tr>
				<td style="text-transform:capitalize; width:225px;"><?php echo(str_replace('_', ' ', $setting['name'])); if($setting['protected']){ echo '<img src="'.common::document_root() .'/skin/transparent-gif.gif" class="sprite sprite-lock" alt="Setting locked icon" title="This setting is locked, any changes will be ignored"/>'; }?></td>
				<td style="text-transform:capitalize; width:210px;">
					<?php if(is_bool($setting['value'])){ ?>
					True: <input type="radio" value="true" name="<?php echo($setting['name']); ?>" id="<?php echo($setting['name'].'_true'); ?>" <?php if($setting['value'] === true){ echo'checked="checked"'; } if($setting['protected']) {echo' disabled="disabled"'; }?> />
					False: <input type="radio" value="false" name="<?php echo($setting['name']); ?>" id="<?php echo($setting['name'].'_false'); ?>" <?php if($setting['value'] !== true){ echo'checked="checked"'; } if($setting['protected']) {echo' disabled="disabled"'; }?> />
					<?php }else if(is_array($setting['options'])){
						echo '<select name="'. $setting['name'] .'" id="'. $setting['name'] .'"'; if($setting['protected']) {echo' disabled="disabled"'; } echo '>'; 
						foreach ($setting['options'] as $option){ ?>
							<option name="<?php echo $option; ?>"<?php if($setting['value'] == $option){ echo' selected="selected"'; }?>><?php echo $option; ?></option>
					<?php } echo '</select>';
					} else {
					?> <input type="text" value="<?php echo($setting['value']); ?>" name="<?php echo($setting['name']); ?>" id="<?php echo($setting['name']); ?>"<?php if($setting['protected']) {echo' disabled="disabled"'; } ?>/>
					<?php }?>
				</td>
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