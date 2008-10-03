<?php
/*
 @author: Taufeeq - Thu Sep 18 13:51:27 PKT 2008::13:51:27
 @package: mysql
 @class:mysql_db_schema_2_xml
 @desc: class to export the complete mysql database schema into xml format
 		it can be used either print/return xml document or can be used to save as
 		xml document

 		compatible for all php versions
 */

class mysql_db_schema_2_xml{

	private $db_handle;
	private $db_name;

	/*
	   db_name must be provided
	   provide either the handle/link to database if the connection is already created
	   or full connection details including the db_host,db_user, db_pass
	*/

	//constructor
	public function mysql_db_schema_2_xml($db_name,$db_handle=null,$db_host=null,$db_user=null,$db_pass=null){
		$this->db_name = $db_name;

		if($db_handle){
			$this->db_handle = $db_handle;
		}else{

			$this->db_handle = mysql_connect($db_host,$db_user,$db_pass)
			or die("Could not connect to database");
			mysql_select_db($this->db_name,$this->db_handle)
			or die("Could not select the database or database doesn't exist");
		}
	}

	//Destructor
	private function _mysql_db_schema_2_xml(){
		mysql_close($this->db_handle);
		unset($this->db_handle,$this->db_name);
	}

	private function getTablesResultSet(){
		return  mysql_query("SHOW TABLES FROM ".$this->db_name ,$this->db_handle);
	}

	private function getTableFieldsResultSet($table){
		//return mysql_query("SHOW FIELDS FROM $table ", $this->db_handle);
		return mysql_query("SHOW FULL COLUMNS FROM `$table` ", $this->db_handle);
	}

	public function schema2XML(){
		$xml = null;
		$tbl_result_set = $this->getTablesResultSet();
		$db_info_res = mysql_query("SHOW VARIABLES WHERE Variable_name LIKE 'character_set_database' OR Variable_name LIKE 'storage_engine' ",$this->db_handle);

		$db_row[0] = mysql_fetch_assoc($db_info_res);
		$db_row[1] = mysql_fetch_assoc($db_info_res);

		//echo "<pre>".print_r($db_row);
		$char_set = $db_row[0]['Value'];
		$engine = $db_row[1]['Value'];

		if($tbl_result_set && (mysql_num_rows($tbl_result_set)>0)){
			$xml = '<?xml version="1.0" ?>';
			$xml .= '<schema charset="'.$char_set.'" engine="'.$engine.'">';
			while ($tbl_row = mysql_fetch_row($tbl_result_set)) {
				$res = mysql_query("SHOW CREATE TABLE ".$tbl_row[0], $this->db_handle);
		  		if($res && (mysql_num_rows($res)>0)){
		  			$struct_row = mysql_fetch_row($res);
		  			$tbl_structure = $struct_row[1];
		  		}
		  		$xml .= '<table name="'.$tbl_row[0].'">';
		  		$xml .= '<structure>'.$tbl_structure.'</structure>';
				$fld_result_set = $this->getTableFieldsResultSet($tbl_row[0]);
				if($fld_result_set && (mysql_num_rows($fld_result_set)>0)){
					while ($fld_row = mysql_fetch_assoc($fld_result_set)){
						$xml .= '<field name="'.strtoupper($fld_row['Field']).'">';
						$xml .= 	'<type>'.$fld_row['Type'].'</type>';
						if($fld_row['Collation'])
							$xml .= '<collation>'.$fld_row['Collation'].'</collation>';

     					if($fld_row['Null'])
							$xml .= '<null>'.$fld_row['Null'].'</null>';

     					if($fld_row['Key'])
     						$xml .='<key>'.$fld_row['Key'].'</key>';

     					if($fld_row['Default'] or ($fld_row['Default']=='0'))
     						$xml .= 	'<default>'.$fld_row['Default'].'</default>';

     					if($fld_row['Extra'])
     						$xml .=		'<extra>'.$fld_row['Extra'].'</extra>';

     					$xml .=		'<privileges>'.$fld_row['Privileges'].'</privileges>';

     					if($fld_row['Comment'])
     						$xml .=		'<comment>'.$fld_row['Comment'].'</comment>';

     					$xml .= '</field>';
					}
				}
				$xml .= '</table>';
			}
			$xml .= '</schema>';
		}
		return $xml;
	}

	public function generateXMLSchema($file_name,$download=false){
		$file = fopen($file_name,'w');
		fwrite($file,$this->schema2XML());
		fclose($file);
		if($download){
			$this->downloadFile($file_name);
		}
		$this->_mysql_db_schema_2_xml();
		return true;
	}

	public function printXMLSchema(){
		header("Content-type: text/xml");
		echo $this->schema2XML();
		$this->_mysql_db_schema_2_xml();
	}

	public function downloadFile($filename){

		// required for IE, otherwise Content-disposition is ignored
		if(ini_get('zlib.output_compression'))
		  ini_set('zlib.output_compression', 'Off');

		$file_extension = strtolower(substr(strrchr($filename,"."),1));

		if( $filename == "" ){
			echo "<html><title>Download Failed</title><body>ERROR: Please provide the file to download.</body></html>";
			exit;
		} elseif ( ! file_exists( $filename ) ){
		    echo "<html><title>Download Failed</title><body>ERROR: Download file not found</body></html>";
		    exit;
		};

		switch( $file_extension ){
		  case "pdf": $ctype="application/pdf"; break;
		  case "exe": $ctype="application/octet-stream"; break;
		  case "zip": $ctype="application/zip"; break;
		  case "doc": $ctype="application/msword"; break;
		  case "xls": $ctype="application/vnd.ms-excel"; break;
		  case "ppt": $ctype="application/vnd.ms-powerpoint"; break;
		  case "gif": $ctype="image/gif"; break;
		  case "png": $ctype="image/png"; break;
		  case "jpeg":
		  case "jpg": $ctype="image/jpg"; break;
		  default: $ctype="application/force-download";
		}
		header("Pragma: public"); // required
		header("Expires: 0");
		header("Cache-Control: must-revalidate, post-check=0, pre-check=0");
		header("Cache-Control: private",false); // required for certain browsers
		header("Content-Type: $ctype");

		header("Content-Disposition: attachment; filename=\"".basename($filename)."\";" );
		header("Content-Transfer-Encoding: binary");
		header("Content-Length: ".filesize($filename));
		readfile("$filename");
	}

}