<?php
class cmspage_search extends cmspage
{
	//Number of results that can_view has returned true for
	private $i = 0;
		
	//Result number in search result
	private $j = 0;
	
	//Number of results going to be returned
	private $n = 0;
	
	//Maximum number of result the db will return
	private $max_results = 0;
	
	private $page = 0;
	private $drop =0;
	
	private	$results = array();
	private $no_of_results = 1;
	private $no_of_results_from_db;
	
	private $search;
	private $search_terms;
	private $search_possible_accro;
	private $search_string_corrected;
	private $search_corrected = '';
	private $search_tags_xml;
	private $search_commands_xml;
	private $types;
	
	private $highlight;
	
	private $time_start;
	
	public function run()
	{
		echo($this->search('search'));
	}
	
	/**
	 * search(string, optional number of results, boolean mode = false)
	 * This is the public method for searching the backend database
	 * $element_types 	NULL = no restrictions
	 * 					1 = not documents
	 * 					2 = elements with files only
	 */
	public function search($string, $element_types = NULL, $no_of_results = 5, $boolean = false, $page = 0, $send_xml_header = true, $highlight = false)
	{	
		$this->time_start = time();
		
		//Clean the input
		$this->search = stripslashes($string);
		
		//Set in the class how many results we want to return
		if($no_of_results < 3)
		{
			$no_of_results = 3;
		}

		$this->no_of_results = $no_of_results;
			
		//build where query for restriction on element types
		$element_sql = '';
		
		//If the results are paged then work out how many results to drop because they would of previously been seen.
		if(is_numeric($page))
		{
			$this->drop = $this->no_of_results * $page;
			$this->page = $page;
		}
		
		if($element_types == 1)
		{
			$element_sql = " AND (";
			
			foreach(Content::$elements_with_files as $element)
			{
				$element_sql = $element_sql." (`type` != '".$element."') AND";
			}
			//Remove the last AND, then close the outer bracket
			$element_sql = substr($element_sql, 0, -3).')';
		}
		
		if($element_types == 2)
		{
			$element_sql = " AND (";
			
			foreach(Content::$elements_with_files as $element)
			{
				$element_sql .= " (`type` = '".$element."') OR";
			}
			//Remove the last AND, then close the outer bracket
			$element_sql = substr($element_sql, 0, -2).')';
		}
		
		$this->build_search_terms_for_highlighting();
//		$this->spellcheck();
		$this->build_accro();

		$this->types = $element_types; 
		$this->highlight = $highlight;
		
		
//		$this->search_extended();
		$results = database::singleton()->sql_search('tbl_elements_data', 'search', 'SAVED');
		print_r($results); die();
		$this->get_search_results($this->search, $element_sql, $boolean);
		return Search::singleton()->build_xml($send_xml_header);
	}
	
	private function search_extended()
	{
		if($this->page == 0 && sizeof($this->search_terms) > 0)
		{
			//Check the tags table for any content or pages that are taged with one of our search terms 
			$search_tags_db = Databases::singleton()->database($GLOBALS['cms_db_name'])->table('vw_tags')->get_records_by_equalto_query(array('tag'), array($this->search_terms), array('tag'));
			$search_tags_db_exact = Databases::singleton()->database($GLOBALS['cms_db_name'])->table('vw_tags')->get_records_by_field('tag', strtolower($this->search), array('tag'));
			
			//Check the command table for any commands that match our search terms
			$search_commands_db = Databases::singleton()->database($GLOBALS['cms_db_name'])->table('tbl_commands')->get_records_by_equalto_query(array('command', 'type'), array($this->search_terms, array('REDIR')), array('command', 'description'));
			
			
			//Build through the database results to produced a concaternated array of useful results
			$search_commands = array();
			$search_tags = array();
			
			foreach($search_commands_db as $command)
			{
				$search_commands[$command['command']] = $command['description'];
			}
			
			foreach($search_tags_db as $tag)
			{
				if(!isset($search_tags[$tag['tag']]))
				{
					$value = 1;
				}
				else
				{
					$value = $search_tags[$tag['tag']]+1;
				}
				$search_tags[$tag['tag']] = $value;
			}
			
			//Sort the results to most popular first
			//Not required on commands as there unique
			arsort($search_tags);
			
			foreach($search_commands as $command => $description)
			{
				$this->search_commands_xml .= '<command><title>'.ucwords($command).'</title><description>'. $description .'</description></command>';
			}
			
			if(isset($search_tags_db_exact[0]))
			{
				$this->search_tags_xml .= '<tag>'.ucwords($search_tags_db_exact[0]['tag']).'</tag>';
			}
			
			foreach($search_tags as $tag => $value)
			{
				//Skip if the tag is the same as the exact match
				if($tag != $search_tags_db_exact[0]['tag'])
				{
					$this->search_tags_xml .= '<tag>'.ucwords($tag).'</tag>';
				}
			}
		}
	}
	
	private function build_search_terms_for_highlighting()
	{
		$this->search_terms = explode(" ", Common::strip_non_alpha_numeric(stripslashes(strip_tags(urldecode($this->search)))));
		$this->search_terms = array_unique(array_filter($this->search_terms));
		$this->search_terms = array_diff( $this->search_terms, common::mysql_stop_words_array() );
	}
	
	private function build_accro()
	{
		if(count($this->search_terms) > 1)
		{
			foreach($this->search_terms as $term)
			{
				$this->search_possible_accro .= $term[0];
			}
			$this->search_terms[] = $this->search_possible_accro;
		}
	}
	
	/**
	 * get_search_results
	 * This function calls the MySQL backend with a freetext search
	 * It calls can_view_element on the id of the returned data
	 * The function returns an array of the results the user is able to view.
	 */
	private function get_search_results($query, $element_sql, $boolean)
	{		
		//Option to insert into SQL to do boolean search, does not apply the more than 50% rule
		if($boolean == false)
		{
			$boolean = '';
		}
		else
		{
			$boolean = ' IN BOOLEAN MODE';
		}
		
		$this->count_max_results_for_query($query, $element_sql, $boolean);
		
		if($this->max_results != 0)
		{
			//I number of database result loops
			$i = 0;
			
			//$this->j is the number of database results that have been looped through
			//Escape if we have all the results asked for
			//The third 'and' is an escape incase an error happens
			//IE the max elements has been reduced since calced previously
			while( ($this->j < $this->max_results) and ($this->n <= $this->no_of_results) and ($i < 40) )
			{
				$results_from_db = $this->get_results_from_db($query, $element_sql, $boolean, ($i*$this->no_of_results), 25);
				$this->loop_through_results($results_from_db);
				
				if($this->j >= $this->max_results)
				{
					$this->end_of_results_array();
					break;
				}
				
				if($this->n >= $this->no_of_results)
				{
					break;
				}
				
				++$i;
			}
		}
		else
		{
			$this->results[0] = $this->no_more_results_array();
		}
		return $this->results;
	}
	
	private function no_more_results_array()
	{
		return array(	'name' => 'No results',
						'id' => '&#032;0',
						'icon' => 'cross.png',
						'search' => 'Your search returned no results',
						'score' => 0);
	}
	
	/**
	 * This function calls the SQL full text search wraped in a count call
	 * the result per query is cached for the session
	 *
	 * @param unknown_type $query
	 */
	private function count_max_results_for_query($query, $element_sql, $boolean)
	{

		$sql_query = "SELECT COUNT(id) FROM vw_element_search_with_valid_pages WHERE ( MATCH(search) AGAINST('".$query."'".$boolean.") ".$element_sql." )";

		$result = Database::singleton()->direct_sql($sql_query); 
		$_SESSION['cms_search'][$query] = $result[0]['COUNT(id)'];
		
		//If the maximum number of results for this query is less than the requested amount then change the ammount to the max possible
		if($_SESSION['cms_search'][$query] < $this->no_of_results)
		{
			$this->no_of_results = $_SESSION['cms_search'][$query];
		}
		
		$this->max_results =  $_SESSION['cms_search'][$query];
	}
	
	private function get_results_from_db($query, $element_sql, $boolean, $lower_limit, $increment)
	{
		$query = mysqli_real_escape_string(Databases::singleton()->database($GLOBALS['cms_db_name'])->connection()->getConnection(), $query );
		//These vars are so we can get extra database result sets if the first set was not enough.
		$upper_limit = $lower_limit + $increment;

		$sql_query = "SELECT *, (MATCH(search) AGAINST('".$query."'".$boolean.") * 10) AS score FROM vw_element_search_with_valid_pages WHERE ( MATCH(search) AGAINST('".$query."'".$boolean.") ".$element_sql." ) ORDER BY score DESC LIMIT ".$lower_limit.", ".$upper_limit."";
		
		$results_from_db = Databases::singleton()->database($GLOBALS['cms_db_name'])->table('vw_element_search_with_valid_pages')->direct_sql($sql_query);
		
		$this->no_of_results_from_db = count($results_from_db);

		return $results_from_db;
	}
	
	private function check_result_page_validity($page_id)
	{
		return(Content::singleton()->get_page_and_check_valid($page_id));
	}
	
	/**
	 * Loops through the inputed results and adds them to the private results store
	 *
	 * @param unknown_type $results_from_db
	 * @param unknown_type $drop
	 */
	private function loop_through_results($results_from_db)
	{	
		$i = 0;
		$ids = array(); //array of page ids, quick hack so the same page is not listed in the results more than once
		
		while ( ($this->n < $this->no_of_results) and ($i < $this->no_of_results_from_db) and ($i < 100))
		{
			$result = $results_from_db[$this->j];
			
			//escapes null rows
			if(is_numeric($result['id']))
			{
				//Check user can view the page
				if(Security::singleton()->can_view($result['page_id']) and $this->check_result_page_validity($result['page_id']))
				{					
					//Call string highlighting process
					if($this->highlight)
					{
						$result['search'] = Search::singleton()->process_search_text($result['search']);
					}
					else
					{
						$result['search'] = '';
					}
					
					if(Content::does_type_have_files($result['type']) )
					{
						$xml = Databases::singleton()->database($GLOBALS['cms_db_name'])->table('tbl_page_elements_data')->get_record_by_id($result['id'], array('data'));
						
						$a = simplexml_load_string('<data>'.utf8_encode($xml['data']).'</data>');
						if($a===FALSE) {
						//It was not an XML string
						} else {
						//It was a valid XML string
							$simplexml = new SimpleXMLElement('<data>'.utf8_encode($xml['data']).'</data>');
							$result['icon'] = Common::icon_lookup($simplexml->mime_type);
							$result['id'] = 'download/' . $result['id'];	
						}				
					}
					else
					{
						//Make sure tha page is not already in the results array
						if(!isset($ids[$result['page_id']]))
						{
							$result['name'] = Content::singleton()->get_page_name($result['page_id']);
							$result['icon'] = 'page_white_text.png';
							$result['id'] = $result['page_id'].'#element'.$result['id'];
							$ids[$result['page_id']] = true;
						}
					}
						
					//Make sure that null data is not passed in to the XML as the javascript breaks
					if($result['name'] == '') { $result['name'] = 'Unknown Page'; }
					else
					{
						$result['name'] = Search::singleton()->process_search_text_simple($result['name']);
					}
					if($result['search'] == '') { $result['search'] = ' '; }
					
					if($this->i >= $this->drop)
					{
						//If we have been requested to drop X number of results do so here
						$result['n'] = $this->n;
						array_push($this->results, $result);
						$this->n++;
					}

					//increment the number of actual results that is going to get returned
					$this->i++;
				}
				
			}
			//These MUST be outside any brackets but inside the while!
			$this->j++;
			++$i;			
		}
	}
	
	private function spellcheck()
	{
		$this->search_corrected = 'false';
		foreach($this->search_terms as $term)
		{
			$corrected = Common::spellcheck($term);
			if($corrected != $term)
			{
				$this->search_corrected = 'true';
			}

			$this->search_string_corrected .= $corrected . ' ';
		}
		$this->search_string_corrected = trim($this->search_string_corrected);
	}
	
	/**
	 * build_xml(array)
	 * Takes the array of results and builds the basic XML structure to be returned to the browser
	 */
	private function build_xml($add_xml_header = true, $end = false)
	{
		$xml = '';
		if($add_xml_header)
		{
			$xml = '<?xml version="1.0" encoding="utf-8"?>';
		}
		$xml .= '
<root starting="'.($this->drop +1).'" finishing="'.($this->drop + $this->n).'" page="'.$this->page.'" rpp="'.$this->no_of_results.'">
	<search>'.$this->search.'</search>'.$this->search_commands_xml.$this->search_tags_xml;
		
	if($this->search_corrected == 'true')
	{
		$xml .= '<search_corrected>'.$this->search_string_corrected.'</search_corrected>
		<surl>'.urlencode($this->search_string_corrected).'</surl>';
	}
		
	$xml .='<types>'.$this->types.'</types>
	<start>'.$this->time_start.'</start>
	<data>';
		if(!isset($this->results[0]))
		{
			$this->results[0] = $this->no_more_results_array();
		}
	
		if($end)
		{
			$this->end_of_results_array();
		}
		
		foreach($this->results as $result)
			{
				$xml = $xml.'
			<result n="'. ($result['n'] + 1) .'">
				<page>'.$result['name'].'</page>
				<id>'.$result['id'].'</id>
				<icon>'.$result['icon'].'</icon>';
				if(strlen($result['search']) > 1) 
				{ 
					$xml .= '<text>'.$result['search'].'</text>'; 
				} 
				else 
				{ 
					$xml .= '<text>.</text>';
				}
				$xml .= '<score>'.round($result['score']).'</score>
			</result>
	'; 
			}
		$xml .= '
	</data>
	<end>'.time().'</end>
</root>';
		
		return Common::tidy_as_xml($xml);
	}
	
	private function end_of_results_array()
	{		
		array_push($this->results, array('name' => 'End of Results',
				'id' => Settings::singleton()->settings['cms_homepage'][0],
				'icon' => 'cross.png',
				'search' => 'End of Results',
				'score' => 0,
				'n' => $this->n));
	}


	private function process_search_text($result)
	{
		
//		$result = strip_tags(Common::clean_text($result, 1,1,false));
		$number_of_letters = 40;
		$escape_output_length = 300;
		$max_output_length = 400;
		$output_text = '...';
		$output_array_store = array();
		$output_array_keys = array();
		
		//word_possition[position in string] => length
		$word_position = array();
		
		foreach($this->search_terms as $term) 
		{
			$termlen = strlen($term);
			$pos=-1;
			$pos_array = array();
			while (($pos=stripos($result,$term,$pos+1))!==false) $pos_array[$i++]=$pos;
			
			foreach($pos_array as $key => $p) 
			{
				$word_position[$p] = $termlen;
			}
		}
		
		ksort($word_position);
		$end_position = 0;
		$max_length = strlen($result);
		
		foreach($word_position as $pos => $len)
		{
			//Number of key words used in this sentance
			$key_words = 1;
			
			//Find the start
			$start_position = $pos - $number_of_letters;
			if($start_position < 0) { $start_position = 0; }
			
			$current_char = '';
			while($current_char != ' ' && $start_position > 0)
			{
				$current_char = $result[$start_position--];
			}
			++$start_position; ++$start_position;
			
			$current_char = '';
			$temp_position = $start_position;
			while($current_char != '. ' && $temp_position < $pos)
			{
				$current_char = $result[$temp_position].$result[++$temp_position];
			}
			if($current_char == '. ') { $start_position = $temp_position; }
			if($start_position < $end_position) 
			{
				//Trim the last three . from the output text as this is part of the same sentance
				//TODO This might need some thinking about now we re-order the output.
				$output_text = substr($output_text, 0, -5); 
				$start_position = $end_position; 
			}			
			
			//Find the end
			$end_position = $pos + $len + $number_of_letters;
			while(key($word_position) < $end_position)
			{
				$end_position = key($word_position) + current($word_position) + $number_of_letters;
				++$key_words;
				if(next($word_position) === false) { break; }
			}

			$current_char = '';
			$i = 0;
			while($current_char != ' ' && $end_position < $max_length && $i < 100)
			{
				$current_char = $result[$end_position++];
				++$i;
			}

			$current_char = '';
			$temp_position = $end_position;
			while($current_char != '. ' && $temp_position > ($pos + $len) )
			{
				$current_char = $result[($temp_position -1)].$result[$temp_position--];
			}
			
			if($current_char == '. ') { $end_position = $temp_position; }
			
			$output_text = substr($result, $start_position, ($end_position - $start_position));
			$output_length = strlen($output_text);

			//Store the array for relevance sorting
			$output_array_store[] = array('length'=> $output_length, 'text'=>$output_text);
			$output_array_keys[] = $key_words;
			
		}
		
		//Sort the output keys into order with the text with the most keys (Most likley to be relevent) first
		arsort($output_array_keys);
		
		$output_length = 0;
		$output_text = '...';
		
		foreach($output_array_keys as $key => $number_of_keys)
		{
			$output_text .= $output_array_store[$key]['text'] .' ... ';
			$output_length = $output_length + $output_array_store[$key]['length'];		
			
			if($output_length > $escape_output_length) { 
				if($output_length > $max_output_length) { $output_text = substr($output_text, 0, $max_output_length).' ...'; }
				break; 
			}
		}

		return Search::singleton()->process_search_text_simple($output_text);
	}
	
	
	private function process_search_text_simple($result)
	{
		$result = strip_tags(Common::clean_text($result, 1,1,false));
		
		foreach($this->search_terms as $term) 
		{
			if(strlen($term) > 0)
			{
				$result = Search::singleton()->str_highlight($result, Common::clean_text($term, 1,1,false));
			}
		}
		return $result;
	}
	
	private function str_highlight($text, $needle, $options = null, $highlight = null)
	{
		/**
		 * Perform a simple text replace
		 * This should be used when the string does not contain HTML
		 * (off by default)
		 */
		define('STR_HIGHLIGHT_SIMPLE', 1);
		
		/**
		 * Only match whole words in the string
		 * (off by default)
		 */
		define('STR_HIGHLIGHT_WHOLEWD', 2);
		
		/**
		 * Case sensitive matching
		 * (off by default)
		 */
		define('STR_HIGHLIGHT_CASESENS', 4);
		
		/**
		 * Overwrite links if matched
		 * This should be used when the replacement string is a link
		 * (off by default)
		 */
		define('STR_HIGHLIGHT_STRIPLINKS', 8);
		
		/**
		 * Highlight a string in text without corrupting HTML tags
		 *
		 * @author      Aidan Lister <aidan@php.net>
		 * @version     3.1.1
		 * @link        http://aidanlister.com/repos/v/function.str_highlight.php
		 * @param       string          $text           Haystack - The text to search
		 * @param       array|string    $needle         Needle - The string to highlight
		 * @param       bool            $options        Bitwise set of options
		 * @param       array           $highlight      Replacement string
		 * @return      Text with needle highlighted
		 */

	    // Default highlighting
	    if ($highlight === null) {
	       // $highlight = '&lt;span&gt;\1&lt;/span&gt;';
	       $highlight = '&lt;strong&gt;\1&lt;/strong&gt;';
	    }
	
	    // Select pattern to use
	    if ($options & STR_HIGHLIGHT_SIMPLE) {
	        $pattern = '#(%s)#';
	        $sl_pattern = '#(%s)#';
	    } else {
	        $pattern = '#(?!<.*?)(%s)(?![^<>]*?>)#';
	        $sl_pattern = '#<a\s(?:.*?)>(%s)</a>#';
	    }
	
	    // Case sensitivity
	    if (!($options & STR_HIGHLIGHT_CASESENS)) {
	        $pattern .= 'i';
	        $sl_pattern .= 'i';
	    }
	
		$needle = (array) $needle;
		foreach ($needle as $needle_s) {
	        $needle_s = preg_quote($needle_s);
	
	        // Escape needle with optional whole word check
	        if ($options & STR_HIGHLIGHT_WHOLEWD) {
	            $needle_s = '\b' . $needle_s . '\b';
	        }
	
	        // Strip links
	        if ($options & STR_HIGHLIGHT_STRIPLINKS) {
	            $sl_regex = sprintf($sl_pattern, $needle_s);
	            $text = preg_replace($sl_regex, '\1', $text);
	        }
	
	        $regex = sprintf($pattern, $needle_s);
			$text = preg_replace($regex, $highlight, $text);
		}
	
	    return $text;
	}
	
	
}
?>