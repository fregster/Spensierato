<?php
/*
	 @author: Taufeeq - Thu Sep 18 13:51:27 PKT 2008::13:51:27
	 @package: mysql
	 @class: mysql_db_xml_2_schema
	 @desc: class to update the mysql database schema from XML file using DOM
 */

class mysql_db_xml_2_schema{

	private $db_handle;
	private $db_name;
	private $tables;
	private $xml_file;
	private $queries;
	public  $error_msg;

	public function mysql_db_xml_2_schema($xml_file,$db_name,$db_handle=null,$db_host=null,$db_user=null,$db_pass=null){
		$this->db_name = $db_name;
		$this->tables = array();
		$this->queries = array();

		$this->xml_file = $xml_file;

		if($db_handle){
			$this->db_handle = $db_handle;
		}else{
			$this->db_handle = mysql_connect($db_host,$db_user,$db_pass)
			or die("Could not connect to database");
			mysql_select_db($this->db_name,$this->db_handle)
			or die("Could not select the database or database doesn't exist");
		}
	}

	private function _mysql_db_xml_2_schema(){
		mysql_close($this->db_handle);
		unset($this->db_handle,$this->db_name,$this->tables,$this->queries);
	}

	private function getTables(){
		$res = mysql_query("SHOW TABLES FROM ".$this->db_name ,$this->db_handle);
		if($res && (mysql_num_rows($res)>0)){
			while ($tbl_row = mysql_fetch_row($res)) {
				$this->tables[] = $tbl_row[0];
				$this->getTableFields($tbl_row[0]);
			}
		}
	}

	private function getTableFields($table){
		$res = mysql_query("SHOW FULL COLUMNS FROM `$table` ", $this->db_handle);
		if($res && (mysql_num_rows($res)>0)){
			while ($fld_row = mysql_fetch_assoc($res)) {
				$this->tables[$table][] = $fld_row;
			}
		}
	}

	public function XML2Schema(){
		$log = "";
		$tbl_fld_temp_data = array();

		if(!is_file($this->xml_file)){
			$this->error_msg = "File not found, may be removed";
			return false;
		}
		$this->getTables();
		//echo '<pre>';print_r($this->tables);
		//echo in_array("columns_privs",$this->tables);
		$doc = new DOMDocument();
		$doc->load($this->xml_file);
		//$xml = file_get_contents($query);
		//$doc->loadXML($xmlstr);
		$root = $doc->firstChild;
		$charset = $root->getAttribute("charset");
		$engine = $root->getAttribute("engine");

	    $xml_tables = $root->getElementsByTagName( "table" );

	  	foreach( $xml_tables as $xml_tbl){

			$xml_tbl_comments = "";

			$xml_tbl_name = $xml_tbl->getAttribute("name");
	  		$xml_flds = $xml_tbl->getElementsByTagName( "field" );
	  		$xml_tbl_struct = $xml_tbl->getElementsByTagName("structure")->item(0)->nodeValue;
			//echo "<br><br>".$xml_tbl_name."<br><br>";
	  		if(in_array($xml_tbl_name, $this->tables)){
	  			//echo "no need to insert";
                //echo "'".$xml_tbl_name."'<br />";
	  			$tbl_fields = $this->tables["$xml_tbl_name"];
				//print_r($tbl_fields);

				//Array to hold all xml fields names to compare with the existing db at the end
				//$xml_fld_names = array();
	  			foreach ($xml_flds as $xml_fld){
	  				$tbl_fld_type_changed = array();
					$xml_fld_key = null;
					$xml_fld_extra = null;
					$xml_fld_nul = null;
					$xml_fld_default = null;
					$xml_fld_collation = null;
					$xml_fld_comments = "";

					//$xml_fld_names[] =
					$xml_fld_name = $xml_fld->getAttribute("name");
					//echo "<br>".($xml_fld_name);
					$xml_fld_type = $xml_fld->getElementsByTagName("type")->item(0)->nodeValue;

					if(is_object($xml_fld->getElementsByTagName("collation")) && is_object( $xml_fld->getElementsByTagName("collation")->item(0))){
						$xml_fld_collation = $xml_fld->getElementsByTagName("collation")->item(0)->nodeValue;
					}
					if(is_object($xml_fld->getElementsByTagName("null")) && is_object( $xml_fld->getElementsByTagName("null")->item(0))){
						$xml_fld_nul = $xml_fld->getElementsByTagName("null")->item(0)->nodeValue;
					}
					if(is_object($xml_fld->getElementsByTagName("key")) && is_object( $xml_fld->getElementsByTagName("key")->item(0))){
						$xml_fld_key = $xml_fld->getElementsByTagName("key")->item(0)->nodeValue;
					}
					if(is_object($xml_fld->getElementsByTagName("default")) && is_object( $xml_fld->getElementsByTagName("default")->item(0))){
						$xml_fld_default=$xml_fld->getElementsByTagName("default")->item(0)->nodeValue;
					}
					if(is_object($xml_fld->getElementsByTagName("extra")) && is_object( $xml_fld->getElementsByTagName("extra")->item(0))){
						$xml_fld_extra = $xml_fld->getElementsByTagName("extra")->item(0)->nodeValue;
					}
					if(is_object($xml_fld->getElementsByTagName("comment")) && is_object( $xml_fld->getElementsByTagName("comment")->item(0))){
						$xml_fld_comments = $xml_fld->getElementsByTagName("comment")->item(0)->nodeValue;
					}
					$found = false;

					//finding if the field already exists
					foreach ($tbl_fields as $tbl_field){
						if($tbl_field['Field'] == $xml_fld_name){
							//echo "<br><< found : ".$xml_fld_name;
							$found = true;
							break;
						}
					}


					// if exists modify it else add it to the table
					if($found){
						/*
							checking if the type of the existing field conflicts with the
							new coming from xml
						*/
						if(!isset($tbl_field['Collation']))$tbl_field['Collation']=null;
						if(!isset($tbl_field['Null']))$tbl_field['Null']=null;
						if(!isset($tbl_field['Key']))$tbl_field['Key']=null;
						if(!isset($tbl_field['Default']))$tbl_field['Default']=null;
						if(!isset($tbl_field['Extra']))$tbl_field['Extra']=null;
						if(!isset($tbl_field['Comment']))$tbl_field['Comment']="";


						if(($tbl_field['Type'] != $xml_fld_type) or
							($tbl_field['Collation'] != $xml_fld_collation) or
							($tbl_field['Null'] != $xml_fld_nul) or
							($tbl_field['Key'] != $xml_fld_key) or
							($tbl_field['Default'] != $xml_fld_default) or
							($tbl_field['Extra'] != $xml_fld_extra) or
							($tbl_field['Comment'] != $xml_fld_comments)
						 ){
						 	/* //for testing only
							 	echo ("<br>".$tbl_field['Type']." >> ". $xml_fld_type)."<br>".
								($tbl_field['Collation'] ." >> ". $xml_fld_collation) ."<br>".
								($tbl_field['Null']." >> ". $xml_fld_nul) ."<br>".
								($tbl_field['Key'] ." >> ". $xml_fld_key) ."<br>".
								($tbl_field['Default'] ." >> ". $xml_fld_default) ."<br>".
								($tbl_field['Extra'] ." >> ". $xml_fld_extra) ."<br>".
								($tbl_field['Comment'] ." >> ". $xml_fld_comments);
							*/

							//echo '<br>'.$tbl_field['Field'].": ".$tbl_field['Type'] ." >> ". $xml_fld_type;

							//if conflict found then add it to the array to process it after loop to avoid redundency

							/*
								first fetch the data from the existing table and put into the temp
								array then update the filed structure and refill the data again
							*/
							//echo $xml_fld_name." changed <br>______";

							/*
							//Hence mysql changes datatype it self so there is no need to do it ur self
							$res = mysql_query("select $xml_fld_name from $xml_tbl_name ", $this->db_handle);
							if($res && (mysql_num_rows($res) > 0)){
								$temp = array();
								while ($data_row = mysql_fetch_row($res)){
									$temp[] = $data_row[0];
								}
								//echo '<pre>'; var_export($temp); echo '</pre>';
								$tbl_fld_temp_data[$xml_tbl_name][$xml_fld_name.":".$xml_fld_type] = $temp;
								//echo '<pre>'; var_export($tbl_fld_temp_data); echo '</pre>';
							}
							*/

							$query = "ALTER TABLE `$xml_tbl_name` CHANGE `$xml_fld_name` `$xml_fld_name` $xml_fld_type ".
							($xml_fld_collation?" COLLATE $xml_fld_collation ":"").
							 ($xml_fld_nul=='NO'?($xml_fld_default?" default $xml_fld_default":'NOT NULL'):($xml_fld_default?" default $xml_fld_default":'NULL'))
							."  $xml_fld_extra  ;";

							$log .= "\n changing type of field $xml_fld_name of table $xml_tbl_name,  \n\n";
							$this->queries[] = $query;
						}



						//echo "<BR>".	$query;

					}else{
						//echo "insert as new field in same table ".$xml_fld_name;
						 $query = "ALTER TABLE `$xml_tbl_name`
						 ADD `$xml_fld_name` $xml_fld_type
						 ".
						 ($xml_fld_nul=='NO'?($xml_fld_default?" default $xml_fld_default":'NOT NULL'):($xml_fld_default?" default $xml_fld_default":'NULL'))
						."  $xml_fld_extra  ;
						";

						$log .= "\n Adding new field in table $xml_tbl_name as $xml_fld_name \n";
						$this->queries[] = $query;
					}

	  			}


	  		}else{
				//echo $xml_tbl_struct;
				$log.= " \n Creating table \n ".$xml_tbl_name." \n \n";
				$this->queries[] = $xml_tbl_struct;
	  		}
	  		//echo "<br>".$xml_tbl_name;

	  	}
	  	//executing queries
	 // 	echo '<pre>';var_export($this->queries);

	 	/*

	 	if(count($tbl_fld_temp_data) > 0){
	 		foreach ($tbl_fld_temp_data as $table=>$table_data){
	 			foreach ($table_data as $field=>$field_data){
					list($field,$type) = split(":",$field);
					$query = "update $table set $field = ".eval("($type)")
	 			}
	 		}
	 	}

	 	*/
	  	if(count($this->queries)>0){

	  		foreach ($this->queries as $query){
				//echo "<br> Executing: ".$query;
	  			mysql_query($query,$this->db_handle);
	  		}

	  	}

	  	/*
	  		After updating the schema now insert the data for that fields
	  		which type has been changed
	  	*/
	  	$this->_mysql_db_xml_2_schema();
	  	return $log;
	}
}
?>