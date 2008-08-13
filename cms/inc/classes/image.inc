<?php
class image
{
	/**
	 * Generate_thumb
	 * This function resizes an image retaining the proportions of an image to the max width or height as specified
	 * It sets defaults of 800x600
	 * The image will be limited on which ever size is reached first.
	 * 
	 * It check if there is a un-scaled copy of the image first if not copies the image and always uses the un-scaled image
	 * This should keep the quality if the image gets scalled down and then up.
	 * 
	 * It works on JPEG, PNG and BMP files
	 * GIF files might not work with latter versions of GD due to copyright
	 */
	public function generate_thumb($elementid, $th_max_width = NULL, $th_max_height = NULL, $quality = 80) 
	{
		//Check if an unscaled image exsists to use as the origonal file
		//If not move the current file to ../unscaled_images
		$error_msg = array();
		$ok = true;
		
		if(filetransfer::Singleton()->check_path(Settings::Singleton()->get_setting('cms_folder_orig_images')) != true)
		{
			$error =1;
			array_push($error_msg, 'Unable to create or write to the archive directory');
		}
		else
		{
		
		$unscaled_image = Settings::Singleton()->get_setting('cms_folder_orig_images').$elementid;
		$old_path = Settings::Singleton()->get_setting('cms_folder_uploads').$elementid;
		if(!is_file($unscaled_image)) 
		{ 
			$ok = copy($old_path, $unscaled_image);
		}
		
			//Check that if the file did not exsist the copy worked
			if($ok) 
			{				
				//Tries to open the image as a jpeg for conversion returns false on fail.
				//This means there is no need to do MIME checking.
				
				$uploadasjpeg = ($this->open_image_as_jpeg($unscaled_image));
				
				if( $uploadasjpeg != false )
				{	
					//die('Resizing image');
					$image_attribs[0] = imagesx($uploadasjpeg);
					$image_attribs[1] = imagesy($uploadasjpeg);
					
					if( ($th_max_width == NULL) or ($th_max_width == ''))
					{
						$th_max_width = $image_attribs[0];
						if($th_max_width > 600) { $th_max_width = 600; }
					}
					
					if( ($th_max_height == NULL) or ($th_max_height == ''))
					{
						$th_max_height = $image_attribs[1];
						if($th_max_height > 400) { $th_max_height = 400; }
					}
					
					//Check to see if resize is required
					if(	($th_max_height == $image_attribs[1]) and ($th_max_width == $image_attribs[0]))
					{
						//There is no resizing required
						return true;
					}
					else
					{	
						$ratio = ($width > $height) ? $th_max_width/$image_attribs[0] : $th_max_height/$image_attribs[1];
						$th_width = $image_attribs[0] * $ratio;
						$th_height = $image_attribs[1] * $ratio;
						
						$im_new = imagecreatetruecolor($th_width,$th_height);
						
						imageAntiAlias($im_new,true); 
						imageCopyResampled($im_new,$uploadasjpeg,0,0,0,0,$th_width,$th_height, $image_attribs[0], $image_attribs[1]);
	
						//Copy the new image to the original file location with the quality
						$resize = imageJpeg($im_new,$old_path,$quality); 
	
						//TODO update database for the element to show that the file is now a JPEG
						return $resize;
					}
				} 
			}
		}
		return false;
	}
	
	private function open_image_as_jpeg ($file) 
	{ 
		//Function for opening an image file and returning the image as a jpeg. Returns false on fail.
	
	    # JPEG: 
	    $im = @imagecreatefromjpeg($file); 
	    if ($im !== false) { return $im; } 
	
	    # GIF: 
	    $im = @imagecreatefromgif($file); 
	    if ($im !== false) { return $im; } 
	
	    # PNG: 
	    $im = @imagecreatefrompng($file); 
	    if ($im !== false) { return $im; } 
	
	    # GD File: 
	    $im = @imagecreatefromgd($file); 
	    if ($im !== false) { return $im; } 
	
	    # GD2 File: 
	    $im = @imagecreatefromgd2($file); 
	    if ($im !== false) { return $im; } 
	
	    # WBMP: 
	    $im = @imagecreatefromwbmp($file); 
	    if ($im !== false) { return $im; } 
	
	    # XBM: 
	    $im = @imagecreatefromxbm($file); 
	    if ($im !== false) { return $im; } 
	
	    # XPM: 
	    $im = @imagecreatefromxpm($file); 
	    if ($im !== false) { return $im; } 
	
	    # Try and load from string: 
	    $im = @imagecreatefromstring(file_get_contents($file)); 
	    if ($im !== false) { return $im; } 
	
	    return false; 
	}
}

?>