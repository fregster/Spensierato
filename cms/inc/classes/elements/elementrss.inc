<?php 
element::register('RSS Feed', 'elementRSS');
class elementRSS extends elementDefault
{
	private $href;
	private $xslt;
	private $rss_data;
	private $rss_html;
	private $cache_time = 600;
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'RSS Feed';
		$this->allowed_tags(array('href' => 'href', 'Use_Internal_template' => 'checkbox', 'Remote_xslt' => 'href'));
		if(Settings::Singleton()->get_setting('uploads_enabled'))
		{
			$this->allowed_tags(array('Local_xslt' => 'file'));
		}
		if (options::singleton()->get_option('admin_mode')) {
			$this->admin_mode_data['image'] = 'rss';
			$this->admin_mode_data['image_edit'] = 'rss_edit';
		}
	}
	
	protected function build_data_xml()
	{
		//We need the href so quickly cache it so we can get the remote RSS feed before we transform the data
		$this->href = $this->data['href'];
		if(isset($this->data['Use_Internal_template']))
		{
			$this->xslt = @simplexml_load_string('<?xml version="1.0" encoding="UTF-8"?>
<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform" >
<xsl:output omit-xml-declaration="yes" encoding="UTF-8" indent="yes" />

	<xsl:template match="/">
		<div class="rss_title"><xsl:value-of select="//title"/></div>
		<xsl:for-each select="//item">
			<xsl:sort select="pubDate"/>
			<div>
				<div class="rss_item_title"><xsl:value-of select="./title"/></div>
				<div class="rss_item_description"><xsl:value-of select="./description"/></div>
			</div>
		</xsl:for-each>
	</xsl:template>
</xsl:stylesheet>');
		}
		else if(isset($this->data['Local_xslt']))
		{
			$this->xslt = @simplexml_load_string(filetransfer::load_file_as_string($this->id));
		}
		else
		{
			$this->xslt = @simplexml_load_string($this->load_remote_data($this->data['Remote_xslt'], $this->cache_time));
		}
		parent::build_data_xml();
	}
	
	private function load_remote_data($url, $maxage)
	{
		$name = 'rss_'.$url;
		$data = persistantcache::Singleton()->get($name);
		if(!$data)
		{
			$data = $this->get_url_contents($url);
			persistantcache::Singleton()->save($name, $data, $maxage);
		}
		return $data; 
	}
	
	protected function transform_data()
	{
		
		$this->rss_data = @simplexml_load_string($this->get_url_contents($this->href)); //Load the RSS as XML to check validity		
					
		if($this->rss_data !== FALSE && $this->xslt !== FALSE) 
		{

			$this->rss_data = $this->rss_data->asXML();
			$this->xslt = $this->xslt->asXML();

			$this->rss_html = render::singleton()->transform_xml_with_xslt($this->rss_data, $this->xslt);
			
			$x=simplexml_load_string($this->data);
			$rss= $x->addChild('rss', ($this->rss_html));
			$rss->addAttribute('type', 'html');
			$this->data = $this->strip_external_xml_decleration($x->asXML()); 
		}
	}
	
	private function strip_external_xml_decleration($xml)
	{
		$stylesheet='<xsl:stylesheet version="1.0" xmlns:xsl="http://www.w3.org/1999/XSL/Transform">
			<xsl:output omit-xml-declaration="yes" indent="yes"/>
			<xsl:strip-space elements="*" />
			
				<xsl:template match="/">
					 <xsl:copy-of select="*"/>
				</xsl:template>
		</xsl:stylesheet>';
		
		return render::singleton()->transform_xml_with_xslt($xml, $stylesheet);
	}
	
	private function get_url_contents($url){
		if(function_exists('curl_init'))
		{
	        $crl = curl_init();
	        $timeout = 3;
	        curl_setopt ($crl, CURLOPT_URL,$url);
	        curl_setopt ($crl, CURLOPT_RETURNTRANSFER, 1);
	        curl_setopt ($crl, CURLOPT_CONNECTTIMEOUT, $timeout);
	        $ret = curl_exec($crl);
	        curl_close($crl);
	        return $ret;
		}
		else
		{
			return file_get_contents($url);
		}
	}
}
?>