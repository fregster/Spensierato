<?php 
if(get::variable('tools'))
{
	$filename = dirname(__FILE__).DS.'tools'.DS.get::variable('tools').'.inc';
	if(file_exists($filename))
	{
		require_once $filename;
	}
}

$rewrite_enabled = check_rewrite();

if(!$rewrite_enabled)
{
	if(!file_exists(DIR_INDEX.'.htaccess')){
		@copy(INCLUDE_PATH.'example.htaccess', DIR_INDEX.'.htaccess');
		$rewrite_enabled = check_rewrite();
	}
}
$background_image = Common::document_root().'/skin/CrunchyBranch_1024x768.png';
?>
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
			div#notices, div.notices { background:white; margin-left:50px; margin-right:50px; margin-top:5px; margin-bottom:5px; padding:5px;}
			div#notices_critical, div.notices_critical{ background:red; border:thin solid darkred; margin-left:5px; margin-right:5px; margin-top:5px; margin-bottom:5px; padding:5px;}
			div#notices_warning, div.notices_warning{ background:LemonChiffon ; border:thin solid gold; margin-left:5px; margin-right:5px; margin-top:5px; margin-bottom:5px; padding:5px;}
			div#notices_information, div.notices_information {background:LightGreen ; border:thin solid green; margin-left:5px; margin-right:5px; margin-top:5px; margin-bottom:5px; padding:5px;}
			div#create { text-align:right;}
			.title{color:white;text-align:center;}
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
function jsCheckHostTypeDetail(){
	if(document.getElementById('db-host').value == 'sqlite'){
		jsHideID('db-host-detail');
	}else{
		jsShowID('db-host-detail');
	}
}
		</script>
	</head>
<body onload="jsShow(); jsHide();">
<div id="frame" style="width:600px; margin-left:auto; margin-right:auto;">
<div><img src="<?php echo($background_image); ?>" style="height:100%; width:100%; z-index: -100; position: absolute; top: 0; left: 0; position: fixed;" alt="Background Image" /></div>
<h1 class="title">Spensierato Install Page</h1>
<?php if(!isset($_GET['install'])) {?>
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
	
		if(count($rec_missing) > 0)
		{
			echo'<div id="notices_warning"><h3>The following packages are not install and are strongly recomended</h3><ul>';
			foreach($rec_missing as $rec)
			{
				echo "<li>$rec</li>";
			}
			echo'</ul></div>';
		}
	
		if(count($opt_missing) > 0)
		{
			echo'<div id="notices_information"><h4>The following packages are not install and are optional but recomended if posible</h4><ul>';
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
		echo'<div class="notices_warning"><p>URL re-writing is not enabled on your webserver, clean URL\'s are unavailable</p></div>';
		$rewrite_enabled = false;
	}
	else
	{
		if(!$rewrite_enabled)
		{
			echo('<div class="notices_warning"><h4>URL re-writing is enabled on the server but NOT setup for this site.</h4>');
			
			if(apache_get_version())
			{
				if(!file_exists(dirname($_SERVER['SCRIPT_FILENAME']).'/.htaccess'))
				{
					echo('<p>Apache and no htaccess file</p><p>You can use the <a href="http://wiki.apache.org/httpd/Htaccess">htaccess</a> file to configure this for you easily at a very slight performance loss</p><p>To do this copy the example.htaccess to '.Common::document_root_folder_only().DS.'.htaccess or setup your vhost with the rewrite rules as defined in the example.htaccess</p>');
					echo('<p>Spensierato may be able to configure Apache automatically. <a href="'.Common::document_root_folder_only().'?tools=enable_htaccess">Autoconfigure Apache</a><p>');
				}
				else
				{
					echo('<p>Apache is NOT setup to use the .htaccess file even though it is in place. Update the Apache config to <a href="http://httpd.apache.org/docs/2.2/mod/core.html#allowoverride">AllowOverides</a> or enable rewrite rules in the vhost</p>');
				}
			}
			echo('</div>');
		}
	}
	
	/*
	//Temp removed as with the public_html folder there is no information that should be browsable via apache unless apache has been mis configured, this needs to be re-implemented
	if(!$rewrite_enabled && !check_inc_restrictions())
	{
		echo('<div class="notices_critical"><h3 style="text-align:center;">**** ! Security Alert ! ****</h3><p>Your webserver is serving .inc files, enable url rewriting and / or block *.inc files from been served</p><h3 style="text-align:center;">**** ! Security Alert ! ****</h3></div>');
	}
	*/
	echo ('</div>');
	echo ('<div class="notices"><p>When your ready <a href="?install=1">Click to Continue</a></p></div>');
}

if(isset($_GET['install'])){
?>

<div id="form" style="margin-left:auto; margin-right:auto;">
<form method="post" action="">
	<div id="users" class="pad-div">
		Create your user for the site
		<div>Username: <input type="text" name="user_username" value="admin_<?php echo(Common::generate_random_string(3)); ?>" /></div>
		<div>Password: <input type="text" name="user_password" value="<?php echo(Common::generate_random_string(8)); ?>" /></div>
		<div>Email: <input type="text" name="user_email" value="root@localhost" /></div> This must be a valid email address
	</div>
	<div id="dbinfo" class="pad-div">
	Please specify the connection information to your SQL server, this user needs to be able to create users and databases, as well as give permissions (Usally root or sa)
		<div>Database Layer: <select name="db-layer" id="db-layer" onchange="jsCheckHostType();"><option value="pdo">PDO (Cross SQL platform - Default)</option><?php if(!array_search('MDB2.php', $opt_missing)) { echo '<option value="mdb2">MDB2 (Default)</option>'; } ?><option value="php">PHP's Builtin Functions (Not recomended)</option></select></div>
		<div id="db-driver">
			<div>Host type: <select id="db-host" name="type" onchange="jsCheckHostTypeDetail();">
				<?php /*TODO:: This need's rewriting to be cleaver about the options shown*/ ?>
				<?php if(isset($pdo_drivers['sqlite']) || class_exists('sqlite3', false)) { ?><option value="sqlite">SQLite (Filesystem based)</option><?php } ?>
				<?php if(class_exists('mysqli', false) || isset($pdo_drivers['mysql'])) { ?><option value="mysqli">MySQL 4+</option><?php } ?>
				<?php if(class_exists('mysql', false)) { ?><option value="mysql">MySQL 4</option><?php } ?>
				<?php if(isset($pdo_drivers['sqlsrv'])) { ?><option value="sqlsrv">MSSQL (MS SQL Driver 2005+)</option><?php } ?>
				<?php if(class_exists('mssql', false)) { ?><option value="mssql">MSSQL (2000, 2005 and 2008)</option><?php } ?>
			</select></div>
		</div>
		<div id="db-host-detail" style="display: none;">
			<div>Host: <input type="text" name="host" value="localhost" /></div>
			<div>Username: <input type="text" name="username" value="root" /></div>
			<div>Password: <input type="password" name="password" /></div>
		</div>
		
		<div id="database_advanced_button" class="js-show button">
			<div class="button_open"><a href="#" onclick="jsShowID('database_advanced'); jsHideID('database_advanced_button'); return false;">Show advanced database options &gt;&gt;&gt;</a></div>
		</div>
		<div id="database_advanced" class="js-hide">
			<div class="button_close"><a href="#" onclick="jsShowID('database_advanced_button'); jsHideID('database_advanced'); return false;">Hide advanced database options &lt;&lt;&lt;</a></div>
			<div id="database" class="pad-div">
				Database to connect to or create
				<div>Database name: <input type="text" name="database" value="spensierato" /></div>
				<div id="prefix">
					If your connecting to an exsisting database this must match the origonal table prefix or new tables will be created
					<div>Table Prefix name: <input type="text" name="prefix" value="<?php echo(Common::generate_random_string(3, false)); ?>" /></div>
				</div>
				
				<div id="db_users" class="pad-div">
					If no usernames are set then one will be created and a random password will be generated (Recomended)<br /><br />
					If the password set is does not meet the recomended length an random password will also be created
					<div>Read only username: <input type="text" name="r_user" value="spensierato_r" /></div>
					<div>Read Write username: <input type="text" name="rw_user" value="spensierato_wr" /></div>
					<div>Read User password: <input type="password" name="db_password" value="<?php echo(Common::generate_random_string(16)); ?>" /></div>
					<div>Write User password: <input type="password" name="db_password_write" value="<?php echo(Common::generate_random_string(16)); ?>" /></div>
				</div>
			</div>
		</div>
	</div>
	
	<div id="filesystem" class="pad-div">
		Please choose where to allow filesystem access. <br/>The site will work with out write access but detailed logging and uploads will be disabled
		<div>Site specific writable folder: <input type="text" name="cms_folder_writable" value="writable" /></div>
		<div id="filesystem-options-button" class="js-show button">
			<div class="button_open"><a href="#" onclick="jsShowID('filesystem-options'); jsHideID('filesystem-options-button'); return false;">Show advanced filesystem options &gt;&gt;&gt;</a></div>
		</div>
		<div id="filesystem-options" class="js-hide">
			<div class="button_close"><a href="#" onclick="jsShowID('filesystem-options-button'); jsHideID('filesystem-options'); return false;">Hide advanced filesystem options &lt;&lt;&lt;</a></div>
			<div>Use custom folders: <input name="cms_folder_use_custom" type="checkbox" /></div>
			<div>Uploads tempory folder: <input type="text" name="cms_folder_uploads" value="writable/uploads/tmp/" /></div>
			<div>Main Download folder: <input type="text" name="cms_folder_downloads" value="writable/uploads/" /></div>
			<div>Origonal Images: <input type="text" name="cms_folder_orig_images" value="writable/uploads/original_images/" /></div>
			<div>Logging Folder: <input type="text" name="cms_folder_logs" value="writable/logs/" /></div>
			<div>Cache Folder: <input type="text" name="cms_folder_cache" value="writable/cache/" /></div>
			<div>Search Index Folder: <input type="text" name="cms_folder_search" value="writable/search/" /></div>
		</div>
	</div>

	<div id="extra-options-button" class="js-show">
		<div class="button_open"><a href="#" onclick="jsShowID('extra-options'); jsHideID('extra-options-button'); return false;">Show Extra options &gt;&gt;&gt;</a></div>
	</div>
	<div id="extra-options" class="js-hide">
		<div class="button_close"><a href="#" onclick="jsHideID('extra-options'); jsShowID('extra-options-button'); return false;">Hide options &lt;&lt;&lt;</a></div>
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
<?php } ?>
</div>
</body>
</html>
<?php exit;