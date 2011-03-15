<?php
element::register('Google Map', 'element_GoogleMap', false, 'world', 'world_edit');
class element_GoogleMap extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'Google Maps';
		$this->allowed_tags(array('latitude' => 'numeric', 'longitude' => 'numeric', 'height' => 'numeric', 'width' => 'numeric'));
		$this->options = array(
				'zoom' => array('4', '8', '10', '15', '17', '20'),
				'type' => array('Road', 'Satellite','Hybrid','Terrain'),
				'render' => array('static', 'dynamic')
			);
	}
	
	protected function add_late_header()
	{
		if(isset($this->data_array['options']['render']) && $this->data_array['options']['render'] == 'dynamic') {
			if(!isset($this->data_array['options']['zoom'])){$this->data_array['options']['zoom'] = 10; }
			$js = 'function initialize_gm_'.$this->id.'() {
    var latlng'.$this->id.' = new google.maps.LatLng('.$this->data_array['latitude'].', '.$this->data_array['longitude'].');
    var mapOptions'.$this->id.' = {
      zoom: '. $this->data_array['options']['zoom'] .',
      center: latlng'.$this->id.',
      mapTypeId: google.maps.MapTypeId.ROADMAP
    };
    var map'.$this->id.' = new google.maps.Map(document.getElementById("map_canvas_'.$this->id.'"),
        mapOptions'.$this->id.');
  
  var marker'.$this->id.' = new google.maps.Marker({
      position: latlng'.$this->id.', 
      map: map'.$this->id.', 
      title:"'.$this->title.'"});
  };	';
			content::singleton()->add_custom_head('style', array('type' => 'text/css'), 'html { height: 100% } body { height: 100%; margin: 0px; padding: 0px } #map_canvas { height: 100% }');
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src'=> 'http://maps.google.com/maps/api/js?sensor=true'));
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript'), $js);
		}
	}
	
	protected function get_data()
	{
		parent::get_data();
		if(!isset($this->data_array['width'])){
			$this->data_array['width'] = 450;
		}
		
		if(!isset($this->data_array['height'])){
			$this->data_array['height'] = 450;
		}
		
	}
}