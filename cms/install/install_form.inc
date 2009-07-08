<!DOCTYPE html PUBLIC "-//W3C//DTD XHTML 1.0 Strict//EN" "http://www.w3.org/TR/xhtml1/DTD/xhtml1-strict.dtd">
<html xmlns="http://www.w3.org/1999/xhtml" xml:lang="en" lang="en">
	<head>
		<meta http-equiv="content-type" content="text/html;charset=UTF-8" />

		<title>Spensierato Install Page</title>

		<style type="text/css">
			body {background-color: transparent;}
			.pad-div { margin-top:6px; margin-bottom: 6px; border:thin solid #000000; padding:5px; background:#DDDDDD;}
			.js-show { display: none; }
			.js-hide {	}
			.button_open { background:LemonChiffon ; border:thin solid gold; margin-left:5px; margin-right:5px; margin-top:5px; margin-bottom:5px; padding:5px;}
			.button_close { background:green ; border:thin solid darkgreen; margin-left:5px; margin-right:5px; margin-top:5px; margin-bottom:5px; padding:5px;}
			div#notices { background:white; margin-left:50px; margin-right:50px; margin-top:5px; margin-bottom:5px; padding:5px;}
			div#notices_critical { background:red; border:thin solid darkred; margin-left:5px; margin-right:5px; margin-top:5px; margin-bottom:5px; padding:5px;}
			div#notices_warning { background:LemonChiffon ; border:thin solid gold; margin-left:5px; margin-right:5px; margin-top:5px; margin-bottom:5px; padding:5px;}
			div#create { text-align:right;}
		</style>
		<script type="text/javascript">
function jsShow(){
var e = document.getElementsByTagName('div');
for (i = 0; i < e.length; i++) {
if (e[i].className.indexOf("js-show") >= 0) { 
e[i].style.display = 'block';
}
}
};
function jsHide(){
var e = document.getElementsByTagName('div');
for (i = 0; i < e.length; i++) {
if (e[i].className.indexOf("js-hide") >= 0) { 
e[i].style.display = 'none';
}
}
};
function jsToggleDisplay(id){
	if(document.getElementById(id).style.display == 'none'){
		jsShowID(id);
	}else{
		jsHideID(id);
	}
};
function jsHideID(id){
	document.getElementById(id).style.display="none";
};
function jsShowID(id){
	document.getElementById(id).style.display="block";
};
function jsCheckHostType(){
	if(document.getElementById('db-layer').value == 'php'){
		jsHideID('db-driver');
	}else{
		jsShowID('db-driver');
	}
};
		</script>
	</head>
<body onload="jsShow(); jsHide();">
<div id="frame" style="width:600px; margin-left:auto; margin-right:auto;">
<div><img src="<?php echo(Common::document_root_folder_only()); ?>/cms/skins/default/graphics/CrunchyBranch_1024x768.png" style="height:100%; width:100%; z-index: -100; position: absolute; top: 0; left: 0; position: fixed;" alt="" /></div>
<div id="notices"><?php
//die(Common::document_root());
if(isset($req_missing[0]) or isset($opt_missing[0]))
{
	echo'Please note the following problems<br/>';

	if(count($req_missing) > 0)
	{
		echo'<div id="notices_critical"><h3>The following packages are not install and are REQUIRED</h3><ul>';
		foreach($req_missing as $req)
		{
			echo "<li>$req</li>";
		}
		echo'</ul></div>';
	}

	if(count($opt_missing) > 0)
	{
		echo'<div id="notices_warning"><h4>The following packages are not install and are recomended</h4><ul>';
		foreach($opt_missing as $opt)
		{
			echo "<li>$opt</li>";
		}
		echo'</ul></div>';
	}

	if(isset($req_missing[0]))
	{
		echo'<p>You may attempt to continue to install but it might not work and even if the install does the is little chance the site will</p><br/>';
	}
}
else
{
	echo'<p>All required and optional PHP extensions are installed</p><br/>';
}

if(!Common::rewrite_enabled())
{
	echo'URL re-writing is not enabled on your webserver, clean URL\'s are unavailable<br/>';
}
?></div>
<div id="form" style="margin-left:auto; margin-right:auto;">
<form method="post" action="">
	<div id="users" class="pad-div">
		Create your user for the site
		<div>Username: <input type="text" name="user_username" value="admin" /></div>
		<div>Password: <input type="text" name="user_password" value="<?php echo(Common::generate_random_string(8)); ?>" /></div>
		<div>Email: <input type="text" name="user_email" value="root@localhost" /></div> This must be a valid email address
	</div>
	<div id="dbinfo" class="pad-div">
	Please specify the connection information to your SQL server, this user needs to be able to create users and databases, as well as give permissions (Usally root or sa)
		<div>Host: <input type="text" name="host" value="localhost" /></div>
		<div>Database Layer: <select name="db-layer" id="db-layer" onchange="jsCheckHostType();"><option value="mdb2">MDB2 (Default)</option><option value="pdo">PDO (Cross SQL platform)</option><option value="php">PHP's Mysql (Not recomended)</option></select></div>
		<div id="db-driver">
			<div>Host type: <select name="type">
				<option value="mysqli">MySQL 4+</option>
				<option value="mysql">MySQL 4</option>
				<option value="mssql">MSSQL (2000, 2005 and 2008)</option>				
			</select></div>
		</div>
		<div>Username: <input type="text" name="username" value="root" /></div>
		<div>Password: <input type="password" name="password" /></div>
		
		<div id="database_advanced_button" class="js-show button">
			<div class="button_open"><a href="" onclick="jsShowID('database_advanced'); jsHideID('database_advanced_button'); return false;">Show advanced database options &gt;&gt;&gt;</a></div>
		</div>
		<div id="database_advanced" class="js-hide">
			<div class="button_close"><a href="" onclick="jsShowID('database_advanced_button'); jsHideID('database_advanced'); return false;">Hide advanced database options &lt;&lt;&lt;</a></div>
			<div id="database" class="pad-div">
				Database to connect to or create
				<div>Database name: <input type="text" name="database" value="spensierato" /></div>
				<div id="prefix">
					If your connecting to an exsisting database this must match the origonal table prefix or new tables will be created
					<div>Table Prefix name: <input type="text" name="prefix" value="<?php echo(Common::generate_random_string(3, false)); ?>" /></div>
				</div>
			</div>
		
			<div id="db_users" class="pad-div">
				If no usernames are set then one will be created and a random password will be generated (Recomended)<br /><br />
				If the password set is does not meet the recomended length an random password will also be created
				<div>Read only username: <input type="text" name="r_user" value="spensierato_read" /></div>
				<div>Read Write username: <input type="text" name="rw_user" value="spensierato_writ" /></div>
				<div>Users password: <input type="password" name="db_password" value="<?php echo(Common::generate_random_string(16)); ?>" /></div>
			</div>
		</div>
	</div>
	
	<div id="filesystem" class="pad-div">
		Please choose where to allow filesystem access. <br/>The site will work with out write access but detailed logging and uploads will be disabled
		<div>Uploads tempory folder: <input type="text" name="cms_folder_uploads" value="writeable/uploads/tmp/" /></div>
		<div>Main Download folder: <input type="text" name="cms_folder_downloads" value="writeable/uploads/" /></div>
		<div>Origonal Images: <input type="text" name="cms_folder_orig_images" value="writeable/uploads/original_images/" /></div>
		<div>Logging Folder: <input type="text" name="cms_folder_logs" value="writeable/logs/" /></div>
		<div>Cache Folder: <input type="text" name="cms_folder_cache" value="writeable/cache/" /></div>
	</div>

	<div id="extra-options-button" class="js-show">
		<div class="button_open"><a href="" onclick="jsShowID('extra-options'); jsHideID('extra-options-button'); return false;">Show Extra options &gt;&gt;&gt;</a></div>
	</div>
	<div id="extra-options" class="js-hide">
		<div class="button_close"><a href="" onclick="jsHideID('extra-options'); jsShowID('extra-options-button'); return false;">Hide options &lt;&lt;&lt;</a></div>
		<div id="randomkey" class="pad-div">
			Create a random key. This is used when generating hashes to help preventing people work out the final hash it defaults to a sha1 hash of the server microtime when you install.
			<div>Random Key: <input type="text" name="random_key" value="<?php echo(sha1(microtime())); ?>" /></div>
		</div>
		<div id="options" class="pad-div">
		<?php if(Common::rewrite_enabled()) {?>
			<div>Enable URL re-writing, <input type="checkbox" name="rewrite" checked="checked" /><br/>Ensure your server has this enabled for this host either by AllowOverride all in the directory config to load the htaccess file or by defining the URL rewriting directly in the directory config</div>
		<?php } ?>
		</div>
		
		<div id="email-options">
			<div>SMTP Host: <input type="text" name="smtp_host" value="localhost" /></div>
			<div>SMTP Port: <input type="text" name="smtp_port" value="25" /></div>
			<div>SMTP Username: <input type="text" name="smtp_username" value="null" /></div>
			<div>SMTP Password: <input type="text" name="smtp_password" value="null" /></div>
		</div>
	</div>

	<div id="create"><input type="submit" value="Create Site" /></div>
</form>
</div>
</div>
</body>
</html>