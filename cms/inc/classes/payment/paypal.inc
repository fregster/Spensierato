<?php
class payment_paypal extends payment
{
	private static $instance;

       private $settings = array(
       'cmd' => '_xclick', 
       'currency_code' => 'GBP', 
       'form_action' => 'https://www.paypal.com/cgi-bin/webscr',
       'image' => 'https://www.paypal.com/en_GB/i/btn/btn_buynowCC_LG.gif', 
       'image_alt' => 'PayPal - The safer, easier way to pay online.',
       'image_tracker' => 'https://www.paypal.com/en_GB/i/scr/pixel.gif',
       'business' => 'QNZGQT236AY3L',
       'tracker' => true,
       );


       public static function Singleton()
       {
          if (!isset(self::$instance)) {
                 $c = __CLASS__;
                 self::$instance = new $c;
          }
          return self::$instance;
       }


       /**
        * 
        * @param $options
        * @return unknown_type
        */

		protected function __construct($options = array())
		{
			$this->settings = array_merge($this->settings, $options);
		}
       
		public function build_html($title, $ammount, $valued_title = NULL, $valued_options = array(), $non_valued_title = NULL, $non_valued_options = array(), $show_tax = false, $tax = 15, $test = false)
		{
			$i = 0;
			$html = '';
			$number_of_valued_items = sizeof($valued_options);
			$number_of_non_valued_items = sizeof($non_valued_options);

			$html .= '<div class="paypal">'.$title.' £'.$ammount.' ';

			if($show_tax)
			{
				$html .= '+ VAT';
			}else{
				$html .= 'INC VAT';
			}

			$html .= '<form action="'.$this->settings['form_action'].'" method="post" target="paypal">';
			if($test)
			{
				$html .= '
<input type="hidden" name="custom" value="test" />';
			}

            $html .= '
<input type="hidden" name="cmd" value="'.$this->settings['cmd'].'" />
<input type="hidden" name="currency_code" value="'.$this->settings['currency_code'].'" />
<input type="hidden" name="item_name" value="'.$title.'" />
<input type="hidden" name="business" value="'.$this->settings['business'].'" />
<input type="hidden" name="amount" value="'.$ammount.'" />
';

              if($show_tax)
              {
                     $html .= '<input type="hidden" name="tax_cart" value="'.$tax.'" />';
              }

              foreach($valued_options as $option => $price)
              {
                     if(is_numeric($price))
                     {
                            $html .= '
<input type="hidden" name="option_amount'.$i.'" value="'.($price+$ammount).'" />
<input type="hidden" name="option_select'.$i.'" value="'.$option.'" />';
                            ++$i;
                     }
              }

              
              if($number_of_non_valued_items > 0)
              {
                     foreach($non_valued_options as $option)
                     {
                            $html .= '
<input type="hidden" name="option_select'.$i.'" value="'.$option.'" />';
                            ++$i;
                     }
              }
                     
              if($number_of_valued_items >= 1)
              {
					$html .= '<div class="paypal_options_chargable">

<input type="hidden" name="on0" value="'.$valued_title.'" />'.$valued_title.'
<select name="os0">';
                     $i = 0;
                     foreach($valued_options as $option => $price)
                     {
                            if(is_numeric($price))
                            {
                                   $html .= '
                     <option value="'.$option.'">'.$option.' + £'. $price.'</option>';
                                   ++$i;
                            }
                     }
                     $html .= '</select>
</div>';
              }
                     
                     if($number_of_non_valued_items > 0)
                     {
                            $html .= '<div class="pappal_options"><input type="hidden" name="on1" value="'.$non_valued_title.'" />'.$non_valued_title.'<select name="os1">';
                            foreach($non_valued_options as $option)
                            {
                                   $html .= '<option value="'.$option.'">'.$option.'</option>';
                                   ++$i;
                            }
                            $html .= '</select>
</div>';
                     }
//            }

              $html .= '<input type="image" src="'.$this->settings['image'].'" style="border:0px;" name="add" alt="'.$this->settings['image_alt'].'" />';
              if($this->settings['tracker'])
              {
            		$html .= '<img alt="" style="border:0px;" src="'.$this->settings['image_tracker'].'" width="1" height="1" />';
              }
              $html .= '</form>';

              $html .='</div>';
              return $html;
       }
       
       
	public function ipn()
	{
		// read the post from PayPal system and add 'cmd'
		$req = 'cmd=_notify-validate';
		
		foreach ($_POST as $key => $value) {
		$value = urlencode(stripslashes($value));
		$req .= "&$key=$value";
		}
		
		// post back to PayPal system to validate
		$header .= "POST /cgi-bin/webscr HTTP/1.0\r\n";
		$header .= "Content-Type: application/x-www-form-urlencoded\r\n";
		$header .= "Content-Length: " . strlen($req) . "\r\n\r\n";
		$fp = fsockopen ('ssl://www.paypal.com', 443, $errno, $errstr, 30);
		
		// assign posted variables to local variables
		$item_name = $_POST['item_name'];
		$item_number = $_POST['item_number'];
		$payment_status = $_POST['payment_status'];
		$payment_amount = $_POST['mc_gross'];
		$payment_currency = $_POST['mc_currency'];
		$txn_id = $_POST['txn_id'];
		$receiver_email = $_POST['receiver_email'];
		$payer_email = $_POST['payer_email'];
		
		if (!$fp) {
		// HTTP ERROR
		} else {
		fputs ($fp, $header . $req);
		while (!feof($fp)) {
		$res = fgets ($fp, 1024);
		if (strcmp ($res, "VERIFIED") == 0) {
		// check the payment_status is Completed
		// check that txn_id has not been previously processed
		// check that receiver_email is your Primary PayPal email
		// check that payment_amount/payment_currency are correct
		// process payment
		}
		else if (strcmp ($res, "INVALID") == 0) {
		// log for manual investigation
		}
		}
		fclose ($fp);
		}
	}
       
}