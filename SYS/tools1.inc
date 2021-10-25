<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : tools1.inc                                                   //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2020-10-12                                                   //
// Author      : FeRox Management Consulting GmbH & Co. KG                    //
//               Adolf-Langer-Weg 11a, D-94036 Passau (Germany)               //
//               https://www.ferox.de - info@ferox.de                         //
// License     : GNU-GPL v3 (https://opensource.org/licenses/GPL-3.0)         //
// -------------------------------------------------------------------------- //
// fx-project - An open source PHP Project Managament Software                //
// Copyright  © FeRox Management Consulting GmbH & Co. KG                     //
// -------------------------------------------------------------------------- //
// This program is free software: you can redistribute it and/or modify       //
// it under the terms of the GNU General Public License as published by       //
// the Free Software Foundation, either version 3 of the License, or          //
// (at your option) any later version.                                        //
//                                                                            //
// This program is distributed in the hope that it will be useful,            //
// but WITHOUT ANY WARRANTY; without even the implied warranty of             //
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the              //
// GNU General Public License for more details.                               //
//                                                                            //
// You should have received a copy of the GNU General Public License          //
// along with this program.  If not, see <https://www.gnu.org/licenses/>.     //
//                                                                            //
// See ../LICENSE.TXT file for more information.                              //
// -------------------------------------------------------------------------- //
// LICENSING ADDENDUM:                                                        //
// Programs in the SPP (Special Programs) subfolder are coded extensions of   //
// the open source software fx-project. These programs are offered for sale   //
// by the manufacturer FeRox Management Consulting GmbH & Co. KG and require  //
// a valid key for execution. It is forbidden to resell these programs        //
// and/or keys or to pass them on free of charge or use them without the      //
// express written permission of FeRox Management Consulting GmbH & Co. KG.   //
////////////////////////////////////////////////////////////////////////////////

/**
 * @file
 * Function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;

$GLOBALS['_carrow']='&nbsp;<br />&nbsp;<font style="color:#cccccc;font-size:10pt;">&#8594;</font>&nbsp;';


////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////////////////////////

/**
 * ???FUNCTION???
 *
 * @param various $arr      - Mandatory parameter:                  ???PARAMETER???
 * @param boolean $with_prg - Optional parameter (default = true):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function urlParameter($arr, $with_prg=true)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$p='';
	if($with_prg)
		$p=basename($_SERVER['PHP_SELF']);

	if(fxIsArray($arr))
	{
		$pc=0;
		foreach($arr as $vn => $vv)
		{
			if(!$pc)
				$p .= '?';
			else
				$p .= '&';

			$p .= $vn.'='.urlencode($vv);
			$pc++;
		}
	}

	return $p;
}

/**
 * ???FUNCTION???
 *
 * @param various $title  - Mandatory parameter:                    ???PARAMETER???
 * @param various $inhalt - Mandatory parameter:                    ???PARAMETER???
 * @param string  $width  - Optional parameter (default = '100%'):  ???PARAMETER???
 * @param string  $height - Optional parameter (default = '100%'):  ???PARAMETER???
 * @param string  $align  - Optional parameter (default = 'left'):  ???PARAMETER???
 * @param string  $button - Optional parameter (default = ''):      ???PARAMETER???
 * @param string  $malign - Optional parameter (default = 'left'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function std_mask($title, $inhalt, $width='100%', $height='100%', $align='left', $button='', $malign='left')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(substr($width, -1) != '%')
		$mowidth=((int)$width+22).'px';
	else
		$mowidth=$width;

	$h  = '<table id="mt0_rahmen" border="0" cellpadding="0" cellspacing="0" align="'.$align.'" style="position:relative;float:none;">'.$GLOBALS['nl'];
	$h .= '	<tr>'.$GLOBALS['nl'];
	$h .= '		<td align="left">'.$GLOBALS['nl'];
	$h .= '			<div class="maskc" style="position:relative;width:'.$mowidth.';white-space:normal;">'.$GLOBALS['nl'];

	$h .= '				<div id="mt0_kopf" class="maskh" style="position:relative;">'.$GLOBALS['nl'];
	$h .= '					<span class="maskht">'.$title.'</span>'.$GLOBALS['nl'];
	$h .= '				</div>'.$GLOBALS['nl'];

	$h .= '				<div class="maskm" style="white-space:normal;">'.$GLOBALS['nl'];
	$h .= '					<table width="100%" border="0" cellpadding="0" cellspacing="0">'.$GLOBALS['nl'];
	$h .= '						<tr>'.$GLOBALS['nl'];
	$h .= '							<td height="'.$height.'" width="100%" align="'.$malign.'">'.$inhalt.'</td>'.$GLOBALS['nl'];
	$h .= '						</tr>'.$GLOBALS['nl'];
	$h .= '					</table>'.$GLOBALS['nl'];
	$h .= '				</div>'.$GLOBALS['nl'];

	if(fxIsArray($button) || strlen($button))
	{
		$h .= '				<div class="maskb" style="position:relative;display:block;">'.$GLOBALS['nl'];
		$h .= '					';
		if(fxIsArray($button))
		{
			$bc=0;
			foreach($button as $btn)
			{
				$width=0;
				if(isset($btn[2]))
					$width=$btn[2];
				if($bc)
					$h .= '&nbsp;';
				$h .= fieldSubmit($btn[0],$btn[1],'sub',$width);
				$bc++;
			}
		}
		else
			$h .= $button;
		$h .= $GLOBALS['nl'];
		$h .= '				</div>'.$GLOBALS['nl'];
	}

	$h .= '			</div>'.$GLOBALS['nl'];
	$h .= '		</td>'.$GLOBALS['nl'];
	$h .= '	</tr>'.$GLOBALS['nl'];
	$h .= '</table>'.$GLOBALS['nl'];

	return $h;
}

/**
 * ???FUNCTION???
 *
 * @param string $prehtml - Optional parameter (default = ''):       ???PARAMETER???
 * @param string $inhalt  - Optional parameter (default = ''):       ???PARAMETER???
 * @param string $valign  - Optional parameter (default = 'middle'): ???PARAMETER???
 * @param string $align   - Optional parameter (default = 'center'): ???PARAMETER???
 */
function std_screen($prehtml='', $inhalt='', $valign='middle', $align='center')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($prehtml)
		echo($prehtml);

	$sinhalt  = '<table width="100%" height="100%" border="0" cellpadding="0" cellspacing="0">';
	$sinhalt .= '	<tr>';
	$sinhalt .= '		<td width="100%" height="100%" align="'.$align.'" valign="'.$valign.'">';
	$sinhalt .= '			'.$inhalt;
	$sinhalt .= '		</td>';
	$sinhalt .= '	</tr>';
	$sinhalt .= '</table>';

	echo($sinhalt);
}

/**
 * ???FUNCTION???
 *
 * @param various $filename - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function execute_file($filename)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$php_string = '';
	$inhalt = fxLoad($filename);
	if(strlen($inhalt))
	{
		$stp = strpos($inhalt, '<?');
		if($stp !== false)
		{
			$etp = strpos($inhalt, '?>', $stp+2);
			if($etp === false)
				$etp = strlen($inhalt);

			if(strtolower(substr($inhalt, $stp+2, 3)) == 'php')
				$php_string = trim(substr($inhalt, $stp+5, $etp-$stp-5));
			else
				$php_string = trim(substr($inhalt, $stp+2, $etp-$stp-2));
		}
	}

	return $php_string;
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function microtime_double()
{
    // Display function call?
    if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

    list($usec, $sec) = explode(' ', microtime());
    return $sec.substr($usec, 1);
}

/**
 * ???FUNCTION???
 *
 * @param various $mtitel - Mandatory parameter:                ???PARAMETER???
 * @param various $mcnt   - Mandatory parameter:                ???PARAMETER???
 * @param string  $cls    - Optional parameter (default = ''):  ???PARAMETER???
 */
function microtime_display($mtitel, $mcnt, $cls='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$diff = $GLOBALS['_FXT'][$mcnt]-$GLOBALS['_FXT'][$mcnt-1];
	if(!strlen($cls))
	{
		if($diff > 2.00)
			$cls = 'lightred';
		else
			$cls = 'lightgreen';
	}

	echo('<br><font class="'.$cls.'">&middot;&nbsp;'.$mtitel.':</font> <font class="s2 lightgrey">'.$GLOBALS['_FXT'][$mcnt].'</font> - <font class="s2">'.$diff.' sec</font>');
}

/**
 * ???FUNCTION???
 *
 * @param various $text - Mandatory parameter:                ???PARAMETER???
 * @param various $dc   - Optional parameter (default = -1):  ???PARAMETER???
 * @param string  $cls  - Optional parameter (default = ''):  ???PARAMETER???
 */
function runtime_display($text, $dc=-1, $cls='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	return;

	$rtbta=debug_backtrace(); $rtbtf=$rtbta[0]['file']; $rtbtl=$rtbta[0]['line'];
	$GLOBALS['_RTC']++; $GLOBALS['_RTT'][$GLOBALS['_RTC']]=microtime_double(); 
	if($dc < 0)
		$dc=$GLOBALS['_RTC']-1;
	else
		echo('<hr class="fxhr" />');
	$diff=$GLOBALS['_RTT'][$GLOBALS['_RTC']] - $GLOBALS['_RTT'][$dc];
	$rtdif=number_format($diff, 9, '.',',');
	if(!strlen($cls))
	{
		if($diff > 2.00)
			$cls='red';
		else
			$cls='blue';
	}
	echo('<font class="lightgrey">&middot; '.$rtbtf.' ('.$rtbtl.')</font> <font class="'.$cls.'">'.$text.':</font> <font class="s2 lightgrey">'.$GLOBALS['_RTT'][$GLOBALS['_RTC']].'</font> - <font class="s2">'.$rtdif.' sec</font><br />');
	if($dc < $GLOBALS['_RTC']-1)
		echo('<hr class="fxhr" />');
}

/**
 * ???FUNCTION???
 *
 * @param various $k - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getKey($k)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($GLOBALS['fxpglobals']['lang'] == 1)
	{
		if($k == 'Shift')
			$k='Umschalt';
		else if($k == 'Ctrl')
			$k='Strg';
		else if($k == 'Del')
			$k='Entf';
		else if($k == '<')
			$k='&lt;';
	}

	return '<font class=black>['.$k.']</font>';
}

/**
 * ???FUNCTION???
 *
 * @param various $mtr  - Mandatory parameter: ???PARAMETER???
 * @param various $mnr  - Mandatory parameter: ???PARAMETER???
 * @param various $mlng - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function maskHelpFile($mtr,$mnr,$mlng)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$id=substr('000000'.$mtr,-6).'_'.substr('000000'.$mnr,-6);
	$path=$GLOBALS['docpath'].substr('00'.$mlng,-2).'/SCR/';

	$hfilename='_'.$id.'.htm';
	$tfilename='_'.$id.'.txt';
	$pfilename='_'.$id.'.png';

	$hfilepath=$path.$hfilename;
	$tfilepath=$path.$tfilename;
	$pfilepath=$path.$pfilename;

	$mfilepath=$path.substr($pfilename,1);

	$ex=0;
	if(file_exists($mfilepath))
		$ex=1;

	$ra=array(
		'id'=>$id,
		'hfilename'=>$hfilename, 'tfilename'=>$tfilename, 'pfilename'=>$pfilename,
		'hfilepath'=>$hfilepath, 'tfilepath'=>$tfilepath, 'pfilepath'=>$pfilepath, 'mfilepath'=>$mfilepath,
		'ex'=>$ex
	);
//fxDebug($ra,'$ra', 0);
	return $ra;
}

/**
 * ???FUNCTION???
 *
 * @param various $msg - Mandatory parameter:                    ???PARAMETER???
 * @param string  $cls - Optional parameter (default = 'grey'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function popupMessage($msg, $cls='grey')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$content='<div class="maskc" style="padding:16px;"><font class="'.$cls.'">'.$msg.'</font>'.closeIcon().'</div>';

	return $content;
}

/**
 * Draw a popup mask
 *
 * @param string  $mtitle      - Mandatory parameter:                      Popup mask title
 * @param string  $htitle      - Optional parameter (default = ''):        Popup content headline
 * @param string  $content     - Optional parameter (default = ''):        Popup content
 * @param string  $id          - Optional parameter (default = ''):        Popup ID
 * @param integer $width       - Optional parameter (default = 0):         Popup mask width in pixel
 * @param integer $height      - Optional parameter (default = 0):         Popup mask height in pixel
 * @param boolean $print_icon  - Optional parameter (default = false):     Display print icon in popup title bar
 * @param boolean $manual_icon - Optional parameter (default = false):     Display user manual icon in popup title bar
 * @param integer $pr          - Optional parameter (default = 8):         Position right in pixel
 * @param integer $pt          - Optional parameter (default = 8):         Position top in pixel
 * @param string  $dragid      - Optional parameter (default = 'iainfo'):  ID of a html element if it should be dragable
 *
 * @return Html string of the popup mask
 */
function popupMask($mtitle, $htitle='', $content='', $id='', $width=0, $height=0, $print_icon=false, $manual_icon=false, $pr=0, $pt=0, $dragid='iainfo')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$di='';
	if(strlen($id))
		$di=' id="'.$id.'" ';

	$width=(int)$width;
	$stw=0;
	$mxw=0;
	if($width > 0)
		$stw=$width;
	if($GLOBALS['fxpglobals']['swidth'] > 0)
		$mxw=$GLOBALS['fxpglobals']['swidth']-120;
	else
		$mxw=1280;

	$height=(int)$height;
	$sth=0;
	$mxh=0;
	if($height > 0)
		$sth=$height;
	if($GLOBALS['fxpglobals']['sheight'] > 0)
		$mxh=$GLOBALS['fxpglobals']['sheight']-128;
	else
		$mxh=640;

	$sc=strlen($content);
	$wha=array();
	if($sc && !$width)
		$wha['width']=0;
	if($sc && !$height)
		$wha['height']=0;
//fxDebug($wha,'$wha: $stw='.$stw.', $sth='.$sth);
	if(sizeof($wha))
	{
		foreach($wha as $wh => $dummy)
		{
			$s=0;
			while(true)
			{
				$p=stripos($content,$wh,$s);
				if($p !== false)
				{
					$p=$p+strlen($wh);
					$i='';
					while($p < $sc)
					{
						$c=substr($content,$p,1);
						$o=ord($c);
						if((($o > 47) && ($o < 58)) || ($c == '%') || ($c == ' ') || ($c == ':') || ($c == '=') || ($c == '"') || ($c == '\''))	// 0-9 or special characters
						{
							if(($o > 47) && ($o < 58))	// 0-9
								$i .= $c;
							else if($c == '%')
								$i='';
							$p++;
						}
						else
							break;
					}
					if(strlen($i))
					{
						$wha[$wh]=max($wha[$wh],(int)$i+32);
//echo($s.' ($p='.$p.'): '.$wha[$wh].'<br />');
						break;
					}
					else
						$s=$p;
				}
				else
					break;
			}
		}
	}
//fxDebug($wha,'$wha: $stw='.$stw.', $sth='.$sth);

	if(isset($wha['width']) && ($wha['width'] > 320))
		$stw=$wha['width'];
	if(isset($wha['height']) && ($wha['height'] > 256))
		$sth=$wha['height'];
	$dw='width:fit-content;';
	if($stw > 0)
		$dw='width:'.$stw.'px;';
	else if(($width >= 0) && ($height >= 0) && ($sth > 0))
		$dw='width:'.$mxw.'px;';
	$dh='';
	if($sth > 0)
		$dh='height:'.$sth.'px;';
	else if(($height >= 0) && ($width >= 0) && ($stw > 0))
		$dh='height:'.$mxh.'px;';

	$html  = '<div'.$di.' class="popup" style="position:relative;padding:6px;white-space:normal;'.$dw.'max-width:'.$mxw.'px;'.$dh.'max-height:'.$mxh.'px;">';

	$html .= '<br />';
	$html .= '	<div style="position:absolute;left:0;top:0;width:100%;height:auto;">';
	$html .= '		<table width="100%" border="0" align="left" cellspacing="0" cellpadding="1">';
	$html .= '			<tr>';
	$html .= '				<td class="pmtitle" width="100%" valign="top" '.fxf_jsFunction('dragIt', '*onmousedown', 'event', $dragid).'>';
	$html .= '				'.$mtitle.'&nbsp;&nbsp;';
	$html .= closeIcon($pr,$pt);
	$pr += 36;
	if($print_icon)
	{
		$html .= printIcon($mtitle,$pr,$pt);
		$pr += 36;
	}
	if($manual_icon)
	{
		$html .= manualIcon($pr,$pt);
		$pr += 36;
	}
	$html .= '				</td>';
	$html .= '			</tr>';
	$html .= '		</table>';
	$html .= '	</div><br />';

	if(strlen($content))
	{
		$html .= '	<div id="fxp_prt_div">';
		$bd='';
		$pl=0;
		$pt=32;
		if(strlen($htitle))
		{
			$bd='border:1px solid #dddddd;border-top:0;';
			$pl=6;
			$hh=20;
			if($width > 0)
			{
				$hw=fxGetHelveticaStringWidth($htitle, 16, 'normal', 'px');
				if($hw > $width)
					$hh += floor($hw/$width)*19;
			}
			$html .= '		<div class="maskh maskht" style="position:absolute;left:'.$pl.'px;top:36px;right:6px;height:'.$hh.'px;margin:0;padding:8px;">'.$htitle.'</div>';
			$pt += $hh+21;
		}
		if($height >= 0)
			$html .= '		<div id="fxp_prt_data" style="position:absolute;left:'.$pl.'px;top:'.$pt.'px;right:6px;bottom:6px;margin:0;padding:6px;'.$bd.'overflow:auto;">';
		else
			$html .= '		<div id="fxp_prt_data" style="position:relative;margin:0;padding:0;'.$bd.'border-bottom-left-radius:8px;border-bottom-right-radius:8px;overflow:hidden;">';
//$html .= '$width='.$width.', $height='.$height.'<br />$stw='.$stw.', $sth='.$sth.'<br />$dw='.$dw.', $dh='.$dh.'<hr />';
		$html .= '			'.$content;
		$html .= '		</div>';
	}

	return $html;
}

/**
 * Draw the user manual icon
 *
 * @param integer $pr - Optional parameter (default = 72):  Icon position right in pixel
 * @param integer $pt - Optional parameter (default = 0):   Icon position top in pixel
 * @param string  $jf - Optional parameter (default = ''):  JavaScript command
 *
 * @return Html string of the user manual icon
 */
function manualIcon($pr=72, $pt=0, $jf='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$manual_literal=meldung(2322,true);	// User Manual

	$id='pop_ic_manual';
	if(!strlen($jf))
		$jf=fxf_jsFunction('loadTR', '^loading', 231, 'newtr');
	$br='';
	if($pr <= 7)
		$br='border-top-right-radius:5px;';

	$html='<img id="'.$id.'" class="sic" src="'.$GLOBALS['gfxpath'].'ic_man_b_36x24.png" tooltip="'.$manual_literal.'" '.$jf.' style="position:absolute;right:'.$pr.'px;top:'.$pt.'px;'.$br.'cursor:pointer;">';

	return $html;
}

/**
 * Draw the print icon
 *
 * @param string  $title - Mandatory parameter:                Title of the generated print document
 * @param integer $pr    - Optional parameter (default = 36):  Icon position right in pixel
 * @param integer $pt    - Optional parameter (default = 0):   Icon position top in pixel
 * @param string  $jf    - Optional parameter (default = ''):  JavaScript command
 *
 * @return Html string of the print icon
 */
function printIcon($title, $pr=36, $pt=0, $jf='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$print_literal=meldung(967,true);	// Print

	$id='pop_ic_print';
	if(!strlen($jf))
		$jf=fxf_jsFunction('print', 'fxp_prt_div', $title);
	$br='';
	if($pr <= 7)
		$br='border-top-right-radius:5px;';

	$html='<img id="'.$id.'" class="sic" src="'.$GLOBALS['gfxpath'].'ic_prt_b_36x24.png" tooltip="'.$print_literal.'" '.$jf.' style="position:absolute;right:'.$pr.'px;top:'.$pt.'px;'.$br.'cursor:pointer;">';

	return $html;
}

/**
 * Draw the close icon
 *
 * @param integer $pr - Optional parameter (default = 0):   Icon position right in pixel
 * @param integer $pt - Optional parameter (default = 0):   Icon position top in pixel
 * @param string  $jf - Optional parameter (default = ''):  JavaScript command
 *
 * @return Html string of the close icon
 */
function closeIcon($pr=0, $pt=0, $h=24, $id='pop_ic_close', $br='', $jf='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$h=(int)$h;
	if($h <= 24)
		$ih=24;
	else
		$ih=36;
	$id=trim($id);
	$close_literal=meldung(968,true);
	if($id == 'pop_ic_close')
		$close_literal .= ' <i class=\'s2 lightergrey\'>(Esc)</i>';	// Close
	$br=trim($br);
	if(!strlen($br) && ($pr <= 7))
		$br='border-top-right-radius:5px;';
	if(!strlen($jf))
		$jf=fxf_jsFunction('fxLinkClose');
	$html='<img id="'.$id.'" class="sic_close" src="'.$GLOBALS['gfxpath'].'ic_cls_b_36x'.$ih.'.png" tooltip="'.$close_literal.'" '.$jf.' onmouseover="$(this).src=\''.$GLOBALS['gfxpath'].'ic_cls_w_36x'.$ih.'.png\';" onmouseout="$(this).src=\''.$GLOBALS['gfxpath'].'ic_cls_b_36x'.$ih.'.png\';" style="position:absolute;right:'.$pr.'px;top:'.$pt.'px;width:36px;height:'.$ih.'px;'.$br.'cursor:pointer;">';

	return $html;
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function checkDB()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Already connected?
	if(isset($GLOBALS['dbid']) && $GLOBALS['dbid'])
		return true;

	// Include basic functions and definitions
	if(!function_exists('db_ini'))
	{
		require('db.inc');
		$GLOBALS['db_noerror']=true;
	}

	// Basic variables
	if(!isset($GLOBALS['datetime']))
		$GLOBALS['datetime']=date('YmdHis', time());
	if(!fxIsArray($GLOBALS['fxpglobals']))
		$GLOBALS['fxpglobals']=array('client'=>0, 'user'=>1, 'person'=>0, 'lang'=>7);
	if(!isset($GLOBALS['fxpglobals']['dbparam']))
		$GLOBALS['fxpglobals']['dbparam']=array();

	// Init DB
	if(!fxIsArray($GLOBALS['fxpglobals']['dbparam']))
	{
		$dberror=db_ini(false);
		if($dberror)
			return false;
	}

	// Establish DB connection
	$GLOBALS['dbid']=db_oeffnen();
	if($GLOBALS['dbid'])
		return true;

	return false;
}

/**
 * ???FUNCTION???
 *
 * @param various $s - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function extractNumberFromString($s)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$s=trim($s);
	if(!strlen($s))
		return 0;
	if((string)((int)$s) === (string)$s)
		return (int)$s;
	$isfloat=false;
	$fnp=-1;
	$lnp=-1;
	for($np=0; $np<strlen($s); $np++)
	{
		$nc=substr($s,$np,1);
		$cc=ord($nc);
		if((($cc > 47) && ($cc < 58)) || ($nc == '.') || ($nc == ',') || ($nc == '+') || ($nc == '-'))
		{
			if($fnp < 0)
				$fnp=$np;
			$lnp=$np;
			if(($nc == '.') || ($nc == ','))
				$isfloat=true;
		}
		else if($lnp >= 0)
			break;
	}
	if($fnp < 0)
		return 0;
	$no=substr($s,$fnp,$lnp+1-$fnp);
	if($isfloat)
		return(get_double($no));
	return((int)$no);
}

/**
 * ???FUNCTION???
 */
function setNewline()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$GLOBALS['nl']="\r\n";
	$GLOBALS['nl_b']= "\r\n";
	$GLOBALS['_os']='windows';
	if(isset($_ENV) && (isset($_ENV['PATH']) && (substr($_ENV['PATH'], 0, 1) == '/') || (isset($_ENV['Path']) && substr($_ENV['Path'], 0, 1) == '/') || (isset($_ENV['path']) && substr($_ENV['path'], 0, 1) == '/')))
		$GLOBALS['_os']='linux';
	else if((isset($_SERVER['PATH']) && (substr($_SERVER['PATH'], 0, 1) == '/')) || (isset($_SERVER['Path']) && (substr($_SERVER['Path'], 0, 1) == '/')) || (isset($_SERVER['path']) && (substr($_SERVER['path'], 0, 1) == '/')))
		$GLOBALS['_os']='linux';
	if($GLOBALS['_os'] != 'windows')
		$GLOBALS['nl_b']="\n";
}

/**
 * ???FUNCTION???
 *
 * @param string $c - Optional parameter (default = ''): ???PARAMETER???
 * @param string $p - Optional parameter (default = ''): ???PARAMETER???
 */
function tGetDesign($c='',$p='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($c))
		$c=(int)$c;
	else
		$c=$GLOBALS['fxpglobals']['client'];
	if(strlen($p))
		$p=(int)$p;
	else
		$p=$GLOBALS['fxpglobals']['person'];
	$filename=$GLOBALS['usrpath'].'C'.substr('0000'.$c,-4).'/P'.substr('000000000'.$p,-9).'/S'.$GLOBALS['lts'].'/t998.inc';
	if(file_exists($filename))
		require($filename);
	else
		$GLOBALS['ps_design']=$GLOBALS['fxpglobals']['design'];

	// Check if css file exists
	if(strlen($GLOBALS['ps_design']))
	{
		$dp=fxpDesignPath($GLOBALS['ps_design']);
		if(!file_exists($dp))
			$GLOBALS['ps_design']='';
	}
}

/**
 * ???FUNCTION???
 *
 * @param various $d - Mandatory parameter:                ???PARAMETER???
 * @param string  $c - Optional parameter (default = ''):  ???PARAMETER???
 * @param string  $p - Optional parameter (default = ''):  ???PARAMETER???
 */
function tSaveDesign($d,$c='',$p='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($c))
		$c=(int)$c;
	else
		$c=$GLOBALS['fxpglobals']['client'];
	if(strlen($p))
		$p=(int)$p;
	else
		$p=$GLOBALS['fxpglobals']['person'];
	$filename=fxSessionFile('t998.inc',$c,$p);
	fxSave($filename, '<'.'?'.'php $GLOBALS[\'ps_design\']=\''.$d.'\'; ?'.'>', true);
}

/**
 * ???FUNCTION???
 *
 * @param various $code - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function colorCode($code)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if($code == 'red')
		$col='e1001a';
	else if($code == 'green')
		$col='009f6b';
	else if($code == 'blue')
		$col='006b9f';
	else if($code == 'grey')
		$col='787878';
	else if($code == 'lightred')
		$col='e87986';
	else if($code == 'lightgreen')
		$col='79c7ae';
	else if($code == 'lightblue')
		$col='79aec7';
	else if($code == 'lightgrey')
		$col='b4b4b4';
	else if($code == 'darkred')
		$col='6f000c';
	else if($code == 'darkgreen')
		$col='004e34';
	else if($code == 'darkblue')
		$col='00344e';
	else if($code == 'darkgrey')
		$col='3b3b3b';
	else
		$col='000000';

	return $col;
}

/**
 * ???FUNCTION???
 *
 * @param various $line - Mandatory parameter:                ???PARAMETER???
 * @param string  $pre  - Optional parameter (default = ''):  ???PARAMETER???
 */
function lineCode($line, $pre='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$chksum=strlen($line);
	for($i=0; $i<strlen($line); $i++)
	{
		$ch=substr($line,$i,1);
		$ca=ord($ch)%32;
		if(($ch == 'a') || ($ch == 'e') || ($ch == 'i') || ($ch == 'o') || ($ch == 'u'))
			$ca *= 2;
		$chksum += $ca;
	}

	return(md5($pre.$chksum));
}

/**
 * ???FUNCTION???
 *
 * @param various $fname - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function convertFilename($fname)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$fname=trim($fname);
	if(!strlen($fname))
		return '';

	$repl=array
	(
		","=>'', " "=>'', "&nbsp;"=>'', "."=>'', "/"=>'', "\\"=>'', "&"=>'', "´"=>'', "'"=>'', "\""=>'',

		"À"=>'a',"Á"=>'a',"Â"=>'a',"Ã"=>'a',"Ä"=>'a',"Å"=>'a',"Æ"=>'ae',"à"=>'a',"á"=>'a',"â"=>'a',"ã"=>'a',"ä"=>'a',"å"=>'a',"æ"=>'ae',
		"È"=>'e',"É"=>'e',"Ê"=>'e',"Ë"=>'e',"è"=>'e',"é"=>'e',"ê"=>'e',"ë"=>'e',
		"Ì"=>'i',"Í"=>'i',"Î"=>'i',"Ï"=>'i',"ì"=>'i',"í"=>'i',"î"=>'i',"ï"=>'i',
		"Ò"=>'o',"Ó"=>'o',"Ô"=>'o',"Õ"=>'o',"Ö"=>'o',"Ø"=>'o',"ò"=>'o',"ó"=>'o',"ô"=>'o',"õ"=>'o',"ö"=>'o',"ø"=>'o',
		"Ù"=>'u',"Ú"=>'u',"Û"=>'u',"Ü"=>'u',"ù"=>'u',"ú"=>'u',"û"=>'u',"ü"=>'u',

		""=>'s',""=>'s',""=>'oe',""=>'oe',""=>'z',""=>'z',""=>'y',"ÿ"=>'y',"Ý"=>'y',"ý"=>'y',"µ"=>'u',"¶"=>'p',
		"Ç"=>'c',"ç"=>'c',"Ð"=>'d',"ð"=>'d',"Ñ"=>'n',"ñ"=>'n',"ß"=>'sz',"Þ"=>'b',"þ"=>'b'
	);
	$fname=strtr(strtolower(strip_tags($fname)), $repl);
//echo('<pre>'.fxHtmlEncode($fame).'</pre>');

	return $fname;
}

/**
 * ???FUNCTION???
 *
 * @return ???RETURN???
 */
function getUnits()
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Währung/Einheiten
	if(!isset($GLOBALS['waehrung']) || !strlen($GLOBALS['waehrung']))
		$GLOBALS['waehrung']=$GLOBALS['fxpglobals']['settings']['waehrung'];
	if(!isset($GLOBALS['currency']) || !strlen($GLOBALS['currency']))
		$GLOBALS['currency']=$GLOBALS['waehrung'];

	$unit_c=$GLOBALS['currency'];					// 
	$unit_hr=meldung(484,true);						// Std.
	$unit_chr=$GLOBALS['currency'].'/'.$unit_hr;	// /Std.
	$unit_h='h';									// h
	$unit_ch=$GLOBALS['currency'].'/'.$unit_h;		// /h
	$unit_d='d';									// d
	$unit_d1=meldung(917,true);						// Tag(e)
	$unit_d2=meldung(1255,true);					// Tage

	$units=array($unit_c, $unit_hr, $unit_chr, $unit_h, $unit_ch, $unit_d, $unit_d1, $unit_d2);
//fxDebug($units,'$units');

	return $units;
}

/**
 * ???FUNCTION???
 *
 * @param various $title     - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $worksheet - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function fxpExcelUTF8($title, $worksheet=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(strlen($title))
	{
		if(FXP_CHARSET != 'UTF-8')
			$title=trim(iconv(FXP_CHARSET, 'UTF-8', str_replace('', chr(164), $title)));
		if($worksheet)
		{
			$title=trim(strtr($title, array('*'=>"#", ':'=>".", '/'=>"|", '\\'=>"|", '?'=>"!", '['=>"(", ']'=>")")));
			if(strlen($title) > 31)
				$title=substr($title,0,28).'...';
		}
	}

	return $title;
}

/**
 * ???FUNCTION???
 *
 * @param various $c - Mandatory parameter:               ???PARAMETER???
 * @param integer $l - Optional parameter (default = 0):  ???PARAMETER???
 */
function fxpExcelCell($c, $l=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$c=max(1,$c);
	$c0=floor(($c-1) / 26);
	$c1=$c - $c0*26;

	$l=max(0,$l);

	if($c0)
		$ec=chr(64+$c0).chr(64+$c1);
	else
		$ec=chr(64+$c1);
//echo('$c='.$c.', $l='.$l.' - $c0='.$c0.', $c1='.$c1.' -- <b>'.$ec.$l.'</b><br />');

	if($l)
		return($ec.$l);
	return($ec);
}

/**
 * ???FUNCTION???
 *
 * @param various $val  - Mandatory parameter:                   ???PARAMETER???
 * @param boolean $from - Optional parameter (default = true):   ???PARAMETER???
 * @param boolean $rnd  - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function convBytes($val, $from=true, $rnd=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Convert (T/G/M/K/B) to B
	if($from === false)
	{
		$n='';
		$u='';
		$f=0;
		for($l=0; $l<strlen($val); $l++)
		{
			$c=strtoupper(substr($val,$l,1));
			$i=(string)((int)$c);
			if($c == '.')
				$f=1;
			else if($c === $i)
				$n .= $i;
			else if(($c === 'B') || ($c === 'K') || ($c === 'M') || ($c === 'G') || ($c == 'T'))
			{
				$u=$c;
				break;
			}
		}
		if($f)
			$n=(float)$f;
		else
			$n=(int)$n;
		if($u == 'T')
			$b=(int)($n*1099511627776);
		else if($u == 'G')
			$b=(int)($n*1073741824);
		else if($u == 'M')
			$b=(int)($n*1048576);
		else if($u == 'K')
			$b=(int)($n*1024);
		else
			$b=(int)$n;
//echo('$val='.$val.' -- $n='.$n.', $u='.$u.', $f='.$f.' -- $b='.$b.'<hr />');
	}
	// Convert B to (T/G/M/K/B)
	else if($from)
	{
		$val=(int)$val;
		$u='';
		if(is_string($from))
		{
			$u=strtoupper(substr(trim($from),0,1));
			if(($u != 'T') && ($u != 'G') && ($u != 'M') && ($u != 'K') && ($u != 'B'))
				$u='';
		}
		if($u === 'T')
			$b=$val/1099511627776;
		else if($u === 'G')
			$b=$val/1073741824;
		else if($u === 'M')
			$b=$val/1048576;
		else if($u === 'K')
			$b=$val/1024;
		else if($u === 'B')
			$b=$val;
		else if($val >= 1099511627776)
		{
			$b=$val/1099511627776;
			$u='T';
		}
		else if($val >= 1073741824)
		{
			$b=$val/1073741824;
			$u='G';
		}
		else if($val >= 1048576)
		{
			$b=$val/1048576;
			$u='M';
		}
		else if($val >= 1024)
		{
			$b=$val/1024;
			$u='K';
		}
		else
		{
			$b=$val;
			$u='B';
		}

		if($rnd)
			$b=ceil($b);
		else
			$b=get_entry(ceil($b*100)/100, 'dezimal');

		$b .= ' '.$u;
		if($u != 'B')
			$b .= 'B';
	}

	return $b;
}

/**
 * ???FUNCTION???
 *
 * @param various $vat_rate - Mandatory parameter:                      ???PARAMETER???
 * @param string  $mode     - Optional parameter (default = 'values'):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getVATRate($vat_rate, $mode='values')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	//echo('<b>getVATRate</b>: $vat_rate='.$vat_rate.', $mode='.$mode.'<br />');
	$vat_rate_normal=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz']);
	$vat_rate_reduced=get_double($GLOBALS['fxpglobals']['settings']['mwst_satz_erm']);
	$mode=substr(strtolower($mode),0,1);
//echo('&rarr; $mode='.$mode.', $vat_rate_normal='.$vat_rate_normal.', $vat_rate_reduced='.$vat_rate_reduced.'<br />');

	// Input
	if($mode == 'i')
	{
		if(is_null($vat_rate))
			$vat_rate='';
		else if(is_string($vat_rate))
		{
			$vat_rate=trim($vat_rate);
			if(strlen($vat_rate))
			{
				$vr1=substr($vat_rate,0,1);
				if($vr1 != '^')
				{
					if(($vr1 == '-') || ($vr1 == '+'))
						$vat_rate='^'.$vr1;
					else
						$vat_rate=get_entry($vat_rate,'dezimal');
				}
			}
		}
	}
	else
	{
		if(is_null($vat_rate))
			$vat_rate=$vat_rate_normal;
		else if(is_string($vat_rate))
		{
			$vat_rate=trim($vat_rate);
			$vr1=substr($vat_rate,0,1);
			if($vr1 == '-')
				$vat_rate=$vat_rate_reduced;
			else if(!strlen($vat_rate) || !is_numeric($vr1))
				$vat_rate=$vat_rate_normal;
			else
				$vat_rate=get_double($vat_rate);
		}
		else
			$vat_rate=get_double($vat_rate);

		if($mode == 'd')	// Decimal
			$vat_rate=get_entry($vat_rate,'dezimal');
	}
//echo('&rarr; $vat_rate=<b>'.$vat_rate.'</b><hr />');

	return $vat_rate;
}

/**
 * ???FUNCTION???
 *
 * @param string $cl - Optional parameter (default = ''): ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getSettingMaxCL($cl='')
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$cl=trim($cl);
	$ra=array('source'=>$cl, 'setting'=>trim($GLOBALS['fxpglobals']['settings']['sel_max_cl']), 'value'=>"", 'chars'=>0, 'lines'=>0);

	if(!strlen($cl))
		$cl=trim($ra['setting']);

	$ic=0;
	$sc=false;
	$ia=array('','');
	for($p=0; $p<strlen($cl); $p++)
	{
		$c=substr($cl,$p,1);
		$o=ord($c);
		if(($o == 45) || (($o > 47) && ($o < 58)))	// 45="-", 48="0", 49="1", ..., 57="9"
		{
			if(($o == 45) && strlen($ia[$ic]))
			{
				$ic++;
				if(!isset($ia[$ic]))
					$ia[$ic]='';
			}
			$ia[$ic] .= $c;
			$sc=false;
		}
		else if(!$sc)								// Separator
		{
			$ic++;
			if(!isset($ia[$ic]))
				$ia[$ic]='';
			$sc=true;
		}
	}
//fxDebug($ia,'$ia: $cl='.$cl);

	$ra['chars']=(int)$ia[0];
	$ra['lines']=(int)$ia[1];

	$ra['value']=$ra['chars'].' / '.$ra['lines'];
//fxDebug($ra,'$ra: getSettingMaxCL()');

	return $ra;
}

/**
 * ???FUNCTION???
 *
 * @param various $pid - Mandatory parameter: ???PARAMETER???
 * @param various $mid - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function get_usersettings($pid, $mid)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	if(!$pid)
		$pid=1;

	$settext=array(1=>'-sprache', 2=>'+formatdatum', 9=>'-deztrennzeichen', 15=>'+waehrung', 21=>'+nkstellen');
	$setdb=array();
	$settings=array();

	// Benutzer-ID
	if($mid && $pid)
		$bid=(int)db_value("SELECT benutzer_id FROM benutzer WHERE personen_id=".$pid." AND mandanten_id=".$mid);
	else
		$bid=0;

	$sql_arr=array("SELECT einstellungs_id, einstellungswert FROM s_einstellungswerte WHERE mandanten_id=0 AND benutzer_id=1 AND einstellungs_id IN (1,2,9,15,21)");
	if($mid)
		$sql_arr[]="SELECT einstellungs_id, einstellungswert FROM s_einstellungswerte WHERE mandanten_id=".$mid." AND benutzer_id=1 AND einstellungs_id IN (1,2,9,15,21)";
	if($mid && $bid)
		$sql_arr[]="SELECT einstellungs_id, einstellungswert FROM s_einstellungswerte WHERE mandanten_id=".$mid." AND benutzer_id=".$bid." AND einstellungs_id IN (1,2,9,15,21)";
	foreach($sql_arr as $sql)
	{
		$values=db_values($sql, '*');
//fxDebug($values, $sql);
		if(is_array($values))
		{
			foreach($values as $val)
				$setdb[$val['einstellungs_id']]=$val['einstellungswert'];
		}
	}

	// Nachschlage-Einstellungen umwandeln
	if(is_array($setdb))
	{
		foreach($setdb as $setnr => $setwert)
		{
			if(substr($settext[$setnr], 0,1) == '+')
				$setwert=meldung((int)$setwert, false);
			$settings[substr($settext[$setnr], 1)]=$setwert;
		}
	}

	return $settings;
}

/**
 * Create an array with all important client data
 *
 * @param integer $cid - Optional parameter (default = 0):  Client ID - if 0 the current client Id is used
 *
 * @return Array with all client data
 */
function fxf_getClientData($cid=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	// Client id
	$cid=fxf_getClientId($cid);

	// Array for client info, unsupported and double functions
	$ca=array();
	$ua=array();
	$da=array();

	// ...Client's id, person id and active status
	$ca['cid']=$cid;
	$ca['pid']=0;
	$ca['act']=true;
	$sql="SELECT personen_id, inaktiv FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$cid;
	$tar=db_values($sql);
	if(fxIsArray($tar))
	{
		$ca['pid']=(int)$tar['personen_id'];
		if(($cid > 1) && $tar['inaktiv'])
			$ca['act']=false;
	}

	// ...Client's default language
	$ca['lng']=fxf_getClientLanguage($cid);

	$backups=array('client'=>$GLOBALS['fxpglobals']['client'], 'lang'=>$GLOBALS['fxpglobals']['lang']);

	// ...Client's program functions, profile groups + users (if client is active)
	$ca['pfa']=array();
	$ca['pga']=array();
	$ca['usr']=array();
	if($ca['act'])
	{
		$GLOBALS['fxpglobals']['client']=$cid;

		$pa=array();
		$ma=loadMenu(true);
		if(fxIsArray($ma))
		{
			foreach($ma as $mi => $m)
			{
				if($m['tr'])
				{
					$tx=$m['tx'];
					$ti=substr($mi,0,-3);
					while(strlen($ti))
					{
						$tx=$ma[$ti]['tx'].' > '.$tx;
						$ti=substr($ti,0,-3);
					}
					$pa[$m['tr']]=array('tx'=>$tx, 'ta'=>$m['ta']);
				}
			}
		}
//fxDebug($pa,'$pa', 0);

		// Program functions
		$sql="SELECT transakt_id, transaktionsname, trans_art FROM trans ORDER BY transakt_id";
		$tar=db_values($sql, '*');
//fxDebug($tar,$sql, 0);
		if(fxIsArray($tar))
		{
			foreach($tar as $t)
			{
				$tri=(int)$t['transakt_id'];
				if(isset($pa[$tri]))
				{
					$tx=$pa[$tri]['tx'];
					$ta=$pa[$tri]['ta'];
				}
				else
				{
					$tx='('.$t['transaktionsname'].')';
					$ta=0;
				}
				$ca['pfa'][$tri]=array('tx'=>$tx, 'ta'=>$ta, 'usm'=>0, 'usf'=>0, 'usp'=>0);
			}
		}

		// Reports
		if(fxIsArray($ca['pfa'][88]))
		{
			$sql="SELECT repid, repname FROM repliste WHERE mandanten_id=".$cid." ORDER BY repid";
			$tar=db_values($sql, '*');
//fxDebug($tar,$sql, 0);
			if(fxIsArray($tar))
			{
				foreach($tar as $t)
				{
					$tri=88000+(int)$t['repid'];
					if(isset($pa[$tri]))
					{
						$tx=$pa[$tri]['tx'];
						$ta=$pa[$tri]['ta'];
					}
					else
					{
						$tx='(R-'.$t['repname'].')';
						$ta=$ca['pfa'][88]['ta'];
					}
					$ca['pfa'][$tri]=array('tx'=>$tx, 'ta'=>$ta, 'usm'=>0, 'usf'=>0, 'usp'=>0);
				}
			}
		}

		// Profile group names
		$pna=array();
		$sql="SELECT wertetabid, tabwert FROM wertetab WHERE id_feld=660 AND mandanten_id=".$cid." AND id_sprache=".$GLOBALS['fxpglobals']['lang']." ORDER BY tabwert";
		$tar=db_values($sql, '*');
		if(fxIsArray($tar))
		{
			foreach($tar as $t)
				$pna[(int)$t['wertetabid']]=$t['tabwert'];
		}
//fxDebug($pna,'$pna: $sql='.$sql, 0);

		// Profile groups
		$pga=array();
		$sql="SELECT benutzergr_id, gruppenname_id FROM benutzergruppen WHERE mandanten_id=".$cid." ORDER BY benutzergr_id";
		$tar=db_values($sql, '*');
		if(fxIsArray($tar))
		{
			foreach($tar as $t)
				$pga[(int)$t['gruppenname_id']]=(int)$t['benutzergr_id'];
		}
//fxDebug($pga,'$pga: $sql='.$sql, 0);

		// Create profile groups array
		// ...Cycle through all names
		if(sizeof($pna))
		{
			foreach($pna as $nid => $nam)
			{
				if(sizeof($pga) && isset($pga[$nid]))
					$ca['pga'][$pga[$nid]]=array('nam'=>$nam, 'nid'=>$nid, 'act'=>1, 'cnt'=>0, 'pfa'=>array());
				else
					$ca['pga'][-$id]=array('nam'=>$nam, 'nid'=>$nid, 'act'=>0, 'cnt'=>0, 'pfa'=>array());
			}
		}
		// Cycle through all groups
		if(sizeof($pga))
		{
			foreach($pga as $nid => $gid)
			{
				if(!isset($ca['pga'][$gid]))
					$ca['pga'][$gid]=array('nam'=>'#'.$gid, 'nid'=>$nid, 'act'=>1, 'cnt'=>0, 'pfa'=>array());
			}
		}

		// Get all program functions for each profile group
		$sql="SELECT benutzergr_id, trans_id, rechte_trans_id FROM rechte_trans WHERE mandanten_id=".$cid." ORDER BY trans_id";
		$tar=db_values($sql, '*');
//fxDebug($tar,$sql, 0);
		if(fxIsArray($tar))
		{
			$gpa=array();
			foreach($tar as $t)
			{
				$tid=(int)$t['trans_id'];
				$gid=(int)$t['benutzergr_id'];
				if(isset($gpa[$gid.'_'.$tid]))
				{
					if(!isset($da[$gid]))
						$da[$gid]=$t['rechte_trans_id'];
					else
						$da[$gid] .= ','.$t['rechte_trans_id'];
					$tid=0;
				}
				else
				{
					$gpa[$gid.'_'.$tid]=true;
					if(fxIsArray($ca['pfa'][$tid]))
					{
						// Outdated or Unauthorized (1=Standard, 10=User, 15=Test, 17=User/Admin, 20=Admin, 27=Admin 1/User, 30=Admin 1, 40=Sysadmin)
						if(!$ca['pfa'][$tid]['ta'] || ($ca['pfa'][$tid]['ta'] == 15) || ((($ca['pfa'][$tid]['ta'] == 27) || ($ca['pfa'][$tid]['ta'] == 30)) && ($cid > 1)) || (($ca['pfa'][$tid]['ta'] == 40) && $GLOBALS['fxpglobals']['person']))
						{
							$da[$tid]=true;
							$tid=0;
						}
					}
					else
					{
						$da[$tid]=true;
						$tid=0;
					}
					if($tid)
						$ca['pga'][$gid]['pfa'][$tid]=true;
				}
			}
		}

		// User's default languages
		$ula=array();
		$sql="SELECT benutzer_id, einstellungswert FROM s_einstellungswerte WHERE einstellungs_id=1 AND benutzer_id>1 AND mandanten_id=".$cid;
		$tar=db_values($sql, '*');
		if(fxIsArray($tar))
		{
			foreach($tar as $t)
				$ula[(int)$sp['benutzer_id']]=(int)$sp['einstellungswert'];
		}
//fxDebug($ula,'$ula: $sql='.$sql, 0);

		// Get client's users
		$sql  = "SELECT b.benutzer_id, b.personen_id, b.name_benutzer, b.e_mail_adresse, b.b_admin, b.login_zaehler, b.status_benutzer,";
		$sql .= " b.punkte, p.personentyp, p.pname, p.vorname, p.geschlecht, p.inaktiv FROM benutzer b, personen p";
		$sql .= " WHERE p.personen_id=b.personen_id AND p.mandanten_id=b.mandanten_id AND b.mandanten_id=".$cid;
		$sql .= " ORDER BY b.name_benutzer";
		$tar=db_values($sql, '*');
//fxDebug($tar,$sql, 0);
		if(fxIsArray($tar))
		{
			foreach($tar as $t)
			{
				// User id
				$id=(int)$t['benutzer_id'];

				// Main administrator?
				if($id == 1)
					$t['b_admin']=1;

				// Default program function types
				// ...Administrator-1
				if(($cid == 1) && ($id == 1))
					$pft=array(1=>true, 17=>true, 20=>true, 30=>true);
				// ...Administrator
				else if(($id == 1) || $t['b_admin'])
					$pft=array(1=>true, 17=>true, 20=>true);
				// ...Default
				else
					$pft=array(1=>true);

				// Status
				$ust=$t['status_benutzer'];
				if(($ust == FXP_USER_INACTIVE) || ($ust == FXP_USER_LOCKED))
					$ust=0;
				else if($t['inaktiv'])
					$ust=-1;
				else
					$ust=1;

				// User language
				if(isset($ula[$id]))
					$ulng=$ula[$id];
				else
					$ulng=$ca['lng'];

				$ca['usr'][$id]=array('unm'=>$t['name_benutzer'], 'pid'=>(int)$t['personen_id'], 'pnm'=>trim($t['pname']), 'pfn'=>trim($t['vorname']), 'eml'=>trim($t['e_mail_adresse']), 'gnd'=>(int)$t['geschlecht'], 'typ'=>(int)$t['personentyp'], 'adm'=>(int)$t['b_admin'], 'lin'=>(int)$t['login_zaehler'], 'sta'=>$ust, 'lng'=>$ulng, 'pft'=>$pft, 'pga'=>array(), 'pfa'=>array());
			}
		}

		// Get profile group assignments
		$sql="SELECT benutzergr_id, benutzer_id FROM benutzergruppenzo WHERE benutzergr_id>=100 AND mandanten_id=".$cid." ORDER BY benutzergr_id";
		$tar=db_values($sql, '*');
//fxDebug($tar,$sql, 0);
		if(fxIsArray($tar))
		{
			foreach($tar as $t)
			{
				$uid=(int)$t['benutzer_id'];
				$gid=(int)$t['benutzergr_id'];

				if(isset($ca['usr'][$uid]))
					$ca['usr'][$uid]['pga'][$gid]=true;
				if(isset($ca['pga'][$gid]))
					$ca['pga'][$gid]['cnt']++;
			}
		}

		// Get user program functions
		if(sizeof($ca['usr']) && sizeof($ca['pfa']))
		{
			foreach($ca['usr'] as $uid => $usr)
			{
				// User active, then check progam function assignment
				if($usr['sta'] == FXP_USER_ACTIVE)
				{
					foreach($ca['pfa'] as $tid => $pfa)
					{
						$fa=false;

						// Is it a user's default program function?
						if(!$fa && isset($usr['pft'][$pfa['ta']]))
							$fa=true;
						// Is it in a user's profile group assignment?
						if(!$fa && sizeof($usr['pga']) && sizeof($ca['pga']))
						{
							foreach($usr['pga'] as $gid => $ga)
							{
								if(fxIsArray($ca['pga'][$gid]) && fxIsArray($ca['pga'][$gid]['pfa']) && isset($ca['pga'][$gid]['pfa'][$tid]))
								{
									$fa=true;
									break;
								}
							}
						}

						// Program function assigned?
						if($fa)
							$ca['usr'][$uid]['pfa'][$tid]=true;
					}
				}
			}
		}
	}

	$GLOBALS['fxpglobals']['client']=$backups['client'];
	$GLOBALS['fxpglobals']['lang']=$backups['lang'];

	// Remove outdated or unsupported program functions
//fxDebug($ua,'$ua', 0);
	if(sizeof($ua))
	{
		$csv=array_to_csv($ua);
		$sql="DELETE FROM rechte_trans WHERE trans_id IN (".$csv.") AND mandanten_id=".$cid;
		@db_x($sql);
	}

	// Remove double program functions
//fxDebug($da,'$da', 0);
	if(sizeof($da))
	{
		foreach($da as $gid => $csv)
		{
			$sql="DELETE FROM rechte_trans WHERE benutzergr_id=".$gid." AND rechte_trans_id IN (".$csv.") AND mandanten_id=".$cid;
			@db_x($sql);
		}
	}

//fxDebug($ca,'$ca', 0);
	return $ca;
}

/**
 * ???FUNCTION???
 *
 * @param various $type - Mandatory parameter: ???PARAMETER???
 * @param various $dv   - Mandatory parameter: ???PARAMETER???
 * @param various $cv   - Mandatory parameter: ???PARAMETER???
 *
 * @return ???RETURN???
 */
function compDouble($type, $dv, $cv)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$dr=round($dv,2);
	$de=get_entry($dr,'dezimal');

	$cr=round($cv,2);
	$ce=get_entry($cr,'dezimal');

	$crm=$cr-0.01;
	$cem=get_entry($crm,'dezimal');

	$crp=$cr+0.01;
	$cep=get_entry($crp,'dezimal');

	$cda=array(
		'dv'=>$dv,	'dr'=>$dr, 'de'=>$de,
		'cv'=>$cv,	'cr'=>$cr, 'ce'=>$ce,	'crm'=>$crm, 'cem'=>$cem,	'crp'=>$crp, 'cep'=>$cep,

		'ok'=>true
	);

	if(($ce != $de) && ($cem != $de) && ($cep != $de))
	{
		$cda['ok']=false;
//fxDebug($cda,'$cda: '.$type);
	}

	return $cda;
}
?>