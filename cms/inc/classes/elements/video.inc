<?php

if(Settings::Singleton()->get_setting('uploads_enabled'))
{
	require_once('image.inc');
	element::register('Video', 'elementVideo');
	class elementVideo extends elementImage
	{
		private $video_extended_details;
		private $preview;
		
		function __construct()
		{
			parent::__construct();
			$this->type = __CLASS__;
			$this->name = 'Video Download';
			$this->image = 'page_white';
			$this->image_edit = 'page_edit';
			$this->video_extended_details = array();
			$this->allowed_tags(array('movietitle', 'rate', 'height', 'width', 'duration', 'copyright'));
		}
		
		protected function add_header()
		{
			$js = Settings::singleton()->get_setting('site_folder_js').'mediaplayer/jquery.pack.js,mediaplayer/flashembed.min.js,mediaplayer/flow.embed.js';
			$flowscript = '$(function() { $("a.flowplayer").flowembed("'.Settings::singleton()->get_setting('document_root').'/file/FlowPlayerDark.swf",  {initialScale:"scale"});	});';
			$css = Settings::singleton()->get_setting('document_root').'/file/media.css';
			
			content::singleton()->add_custom_head('link', array('rel' => 'stylesheet', 'type' => 'text/css', 'href' => $css));
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript', 'src'=> $js));
			content::singleton()->add_custom_head('script', array('type' => 'text/javascript'), $flowscript);
		}
		
		
		protected function attach_upload()
		{
			if(parent::attach_upload())
			{
				if(class_exists('ffmpeg_movie'))
				{
					$path = $this->convert_upload();
					if($path !== false)
					{
						$movie = new ffmpeg_movie($path);
						if($movie != false)
						{
							$this->preview = $movie->getFrame(1);
							$this->create_preview();
							
							$this->video_extended_details['movietitle'] = $movie->getTitle();
							$this->video_extended_details['rate'] = $movie->getBitRate();
							$this->video_extended_details['height'] = $movie->getFrameHeight();
							$this->video_extended_details['width'] = $movie->getFrameWidth();
							$this->video_extended_details['duration'] = $movie->getDuration();
							$this->video_extended_details['author'] = $movie->getAuthor();
							$this->video_extended_details['author'] = $movie->getCopyright();
							return true;
						}				
					}
				}
				else
				{
					return true;
				}
			}
			return false;
		}
		
		protected function save_preview()
		{
			return true;
			//return parent::save_preview();
		}
		
		protected function add_extra_info()
		{
			return array_merge(parent::add_extra_info(), $this->video_extended_details);
		}
		
		private function convert_upload()
		{
			$result = true;
			$output = array();
			$temp_file = Settings::singleton()->get_setting('cms_folder_uploads').$this->id.'-'.CMS::Singleton()->time().'-'.User::Singleton()->id().'.flv';
			$file = Settings::singleton()->get_setting('cms_folder_uploads').$this->id.'-'.$this->revision;
			$command = $this->convert_file_command($file, $temp_file);
		
			//Execute the command
			exec(escapeshellcmd($command), $output, $result);
		
			if($result != false)
			{			
				unlink($file);
				rename($temp_file, $file);
				return $file;
			}
		
			return false;
			
		}
		
		protected function convert_file_command($file, $temp_file)
		{
			return 'ffmpeg -i '.$file.' -vcodec flv '.$temp_file;
		}
		
		private function create_preview()
		{
			if ($this->preview) 
			{
				$image = $this->preview->toGDImage();
				if ($image) 
				{
					$file = Settings::singleton()->get_setting('cms_folder_downloads').$id.'-preview';
					$preview = imagepng($image,$file,9);
					imagedestroy($image);
					return $preview;
				}
			}
			return false;
		}
	}
}
?>