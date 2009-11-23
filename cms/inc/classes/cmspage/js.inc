<?php
class cmspage_js extends cmspage
{
	public function initalise()
	{
		$this->value = Common::clean_and_combine_url(CMS::Singleton()->path_vars, 1);
		return true;
	}
	
	public function run()
	{
		$this->get_js($this->value);
	}
	
	private function get_js($path)
	{
		$try = Settings::Singleton()->get_setting('cms_root').'/js/'.Common::clean_path($path);
		if(is_file($try))
		{
			$this->send_js_file($try);
			exit;
		}

		$try = Settings::Singleton()->get_setting('cms_root') . '/publicfiles/'.Common::clean_path($path);
		if(is_file($try))
		{
			$this->send_js_file($try);
			exit;
		}
		die('404 '.$path);
	}
	
	private function send_js_file($file)
		{
			// javascript header
			header('Content-type: text/javascript');

			switch (Settings::Singleton()->get_setting('js_minify'))
			{
				case '1':
					//Use internal code minification
					echo(Common::remove_js_comments(file_get_contents($file)));
				    break;

				case '2':
					//Use JSMini
					echo JSMin::minify(file_get_contents($file));
				    break;

				default:
				    render::Singleton()->render_file($file);
			}

			exit;
		}
}