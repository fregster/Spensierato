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
		if(get::variable('ajax', 'isset'))
		{
			header('content-type: text/xml');
			die(search::singleton()->search($_GET['searchtext']));
		}
		
		$element = Element::type('elementSearchResults');
		$element->build('results', common::strip_external_xml_decleration(search::singleton()->search($_GET['searchtext'])));
		Render::Singleton()->render_html(Content::Singleton()->build());
	}	
}