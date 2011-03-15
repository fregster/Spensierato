<?php
/**
 * http://apiblog.youtube.com/2010/07/new-way-to-embed-youtube-videos.html
 * <iframe class="youtube-player" type="text/html" width="640" height="385" src="http://www.youtube.com/embed/VIDEO_ID" frameborder="0"></iframe>
 */

element::register('You Tube Embeded Video', 'element_YouTube', false, 'film', 'film_edit');
class element_YouTube extends element_Default
{
	public function __construct()
	{
		parent::__construct();
		$this->type = __CLASS__;
		$this->name = 'You Tube Embeded Video';
		$this->allowed_tags(array('video_id' => 'text', 'width' => 'numeric', ));
	}
}