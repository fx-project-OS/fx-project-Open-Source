<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : design_functions.inc                                         //
// Version     : 24.1                                                         //
// Begin       : 2021-02-01                                                   //
// Last Change : 2024-03-14                                                   //
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
 * PF 40: Design - Function collection
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */


/**
 * Get array with editor tooltip texts
 *
 * @param integer $lang - Optional parameter (default = 0):  Language id, or 0 for current language
 *
 * @return Array with editor tooltip texts
 */
function getDesignEditorTooltips($lang=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lang=(int)$lang;
	if(!$lang)
		$lang=$GLOBALS['fxpglobals']['lang'];

	if($lang == 1)
	{
		$lta=array(
			'dir'	=> "Richtung des Farbverlaufs abändern",
			'add'	=> "Neuen Farbwert hinzufügen",
			'chg'	=> "Diesen Farbwert abändern",
			'del'	=> "Diesen Farbwert löschen",
			'carea'	=> "Farbkategoriebereiche 1-<!--#SIZE#--> der Hauptmenütoolbar erstellt aus <!--#COLS#--> inkl. der Untermenütoolbars.",
			'col1'	=> "der Farbe #1",
			'cols'	=> "den Farben #1-<!--#SIZE#-->",
			'cdel'	=> "Diese Kategoriebereichsfarbe löschen",
			'cadd'	=> "Neue Kategoriebereichsfarbe hinzufügen",
			'pdsp'	=> "Menüvorschau für Farbkategoriebereich #<!--#CSN#--> anzeigen"
		);
	}
	else
	{
		$lta=array(
			'dir'	=> "Change direction of the color gradient",
			'add'	=> "Add new color value",
			'chg'	=> "Change this color value",
			'del'	=> "Delete this color value",
			'carea'	=> "Color category sections 1-<!--#SIZE#--> of the main menu toolbar created from <!--#COLS#--> incl. submenu toolbars.",
			'col1'	=> "color #1",
			'cols'	=> "colors #1-<!--#SIZE#-->",
			'cdel'	=> "Delete this category section color",
			'cadd'	=> "Add new category section color",
			'pdsp'	=> "Display menu preview for category section #<!--#CSN#-->"
		);
	}

	return $lta;
}


/**
 * Get information array of a field in the design style array
 *
 * @param array   $dsa   - Mandatory parameter:               Array containing the design styles
 * @param string  $field - Mandatory parameter:               Key in array (= field name) to be displayed
 * @param integer $lang  - Optional parameter (default = 0):  Language id, or 0 for current language
 *
 * @return Array containing description and html code of a field in the design style array
 */
function getDesignEditor($dsa, $field, $lang=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$lang=(int)$lang;
	if(!$lang)
		$lang=$GLOBALS['fxpglobals']['lang'];
	$lta=getDesignEditorTooltips($lang);

	$ew=640;	// Mask width
	$cw=540;	// Colum width
	$lw=140;	// Literal width
	if(function_exists('db_value'))
	{
		$ew=(int)db_value("SELECT breite FROM masken WHERE nummer=15 AND mandanten_id=0");	// 640
		$cw=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=15 AND eingabefeld=38 AND anzeigeart=5");	// 540
	}

	$a=array('dsc'=>"", 'set'=>"");

	// dsc: Beschreibung
	$d=$dsa[$field]['desc'];
	$a['dsc']=$d;

	// set: Einstellung
	$s='<table width="100%" border="0" cellpadding="0" cellspacing="0">';
	$trc=0;
	foreach($dsa[$field] as $skey => $svalue)
	{
		if(($skey != 'name') && ($skey != 'desc') && ($svalue !== false))
		{
//fxDebug($svalue,$field.': '.$skey, 0);
			if($trc)
				$s .= '<tr><td colspan="2" style="height:4px;"></td></tr>';

			$tw=$cw-$lw;

			$s .= '<tr>';
			$s .= '<td nowrap>&middot;&nbsp;'.$skey.'</td>';
			$s .= '<td width="'.$tw.'" align="right" nowrap><div style="position:relative;margin:0;padding:0;">';
			if(substr((string)$skey,-1) == '*')
			{
				$ci=2;
				$cd='bottom';
				$ca='&darr;';
				if(substr((string)$svalue,1,1) == '|')
				{
					$cda=fxpGetGradientDirection(substr((string)$svalue,0,1));
					if(sizeof($cda) == 3)
					{
						$ci=$cda[0];
						$cd=$cda[1];
						$ca=$cda[2];
					}
					$svalue=substr((string)$svalue,2);
				}

				$svalues=explode(',',$svalue);
//$svalues=array('#006b9f');
				$vmax=sizeof($svalues);
				$tw -= ($vmax*21);
				if($vmax < 8)
					$tw -= 21;
				else
					$tw += 18;
				if($vmax > 1)
				{
					$tw -= 27;
					$s .= '<div id="set_'.$field.'_'.$skey.'_dir" cdir="'.$ci.'" tooltip="'.$lta['dir'].'" style="position:absolute;left:0;top:0;width:20px;height:27px;margin:0;margin-right:6px;text-align:center;font-size:19px;font-weight:bolder;color:#000;background:#fcfcfc;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';cursor:pointer;display:inline-block;" '.fxf_jsFunction('designChangeColorGradient', 'set_'.$field.'_'.$skey, 0, 'event').'>'.$ca.'</div>';

					$tw=floor($tw/$vmax);
				}
				foreach($svalues as $v => $val)
				{
					if($v)
						$s .= '<div class="grey" style="position:relative;top:-4px;padding:2px;display:inline-block;">&rarr;</div>';
					// Change this color value
					$s .= '<div style="position:relative;display:inline-block;">';
					$s .= '<input class="fxftx" type="color" id="set_'.$field.'_'.$skey.'['.$v.']" name="set_'.$field.'_'.$skey.'['.$v.']" tabindex="0" fxform="_fxform_datmask_3" value="'.formatHTMLColor($val, true).'" tooltip="'.$lta['chg'].': <b>'.$val.'</b>" style="width:'.$tw.'px;padding:2px;margin:0;border:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';cursor:pointer;">';
					if($v)
						$s .= '<img src="'.$GLOBALS['gfxpath'].'del_c_80x80.png" tooltip="'.$lta['del'].': <b>'.$val.'" style="position:absolute;top:-7px;right:-8px;height:18px;background:#fcfcfc;border:1px solid #fff;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';border-radius:10px;cursor:pointer;" '.fxf_jsFunction('designField', 'set_'.$field.'_'.$skey.'['.$v.']', 'del', 'event').'>';
					$s .= '</div>';
				}
				// Add new color value
				if($vmax < 8)
				{
					$s .= '<div class="grey" style="position:relative;top:-4px;padding:2px;display:inline-block;">&rarr;</div>';
					$s .= '<div tooltip="'.$lta['add'].'" style="position:relative;top:-2px;width:20px;height:27px;margin:0;text-align:center;font-size:19px;font-weight:bolder;color:#009f6b;background:#fcfcfc;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';cursor:pointer;display:inline-block;" '.fxf_jsFunction('designField', 'set_'.$field.'_'.$skey.'['.($v+1).']', 'add'.$val, 'event').'>+</div>';
				}
			}
			else
			{
				// Change this color value
				$s .= '<input class="fxftx" type="color" id="set_'.$field.'_'.$skey.'" name="set_'.$field.'_'.$skey.'" tabindex="0" fxform="_fxform_datmask_3" value="'.formatHTMLColor($svalue, true).'" tooltip="'.$lta['chg'].': <b>'.$svalue.'</b>" style="width:'.($tw-4).'px;padding:2px;margin:0;border:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';cursor:pointer;">';
			}
			$s .= '</div></td>';
			$s .= '</tr>';

			$trc++;
		}
	}
	$s .= '</table>';
//echo('<pre>'.fxHtmlEncode($s).'</pre>');
	$a['set']=$s;

	return $a;
}

/**
 * ???FUNCTION???
 *
 * @param various $dsa  - Mandatory parameter:                  ???PARAMETER???
 * @param string  $msa  - Optional parameter (default = '00'):  ???PARAMETER???
 * @param integer $ms   - Optional parameter (default = 0):     ???PARAMETER???
 * @param integer $lang - Optional parameter (default = 0):     ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getMenuDesignEditor($dsa, $msa='00', $ms=0, $lang=0)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$a=array('dsc'=>"", 'col'=>"", 'prv'=>"");

	$msai=(int)$msa;
	$msa=substr('0'.$msai,-2);

	$ms=(int)$ms;
	if(!$ms)
	{
		$ma=loadMenu(true);
		foreach($ma as $id => $sa)
		{
			if(!$sa['lv'] && ($sa['tx'] != 'Sysadmin'))
				$ms++;
		}
	}

	$lang=(int)$lang;
	if(!$lang)
		$lang=$GLOBALS['fxpglobals']['lang'];
	$lta=getDesignEditorTooltips($lang);

	// ...Menu colors
	$m0col=$dsa['cmenu1-i']['background'];
	$m1col=array();
	for($m=1; $m<=$ms; $m++)
	{
		if(isset($dsa['cmenu-c'.$m]) && isset($dsa['cmenu-c'.$m]['background']) && strlen((string)$dsa['cmenu-c'.$m]['background']) && ($dsa['cmenu-c'.$m]['background'] != 'transparent'))
			$m1col[]=$dsa['cmenu-c'.$m]['background'];
	}
	if(!sizeof($m1col))
		$m1col=array('#006b9f');
//$m1col=array('#5b9dbd', '#2fb96c', '#d4c53c', '#b33d8c');
//fxDebug($m1col,'$m1col', 0);
	$sm1=sizeof($m1col);
	$mca=getMenuColors($ms, $m0col, $m1col);
//fxDebug($mca,'$mca', 0);

	$ew=640;	// Mask width
	$cw=540;	// Colum width
	if(function_exists('db_value'))
	{
		$ew=(int)db_value("SELECT breite FROM masken WHERE nummer=15 AND mandanten_id=0");	// 640
		$cw=(int)db_value("SELECT spaltenanzahl FROM maskenfelder WHERE nummer=15 AND eingabefeld=38 AND anzeigeart=5");	// 540
	}

	// dsc: Description
	if($sm1 < 2)
		$rcols=$lta['col1'];	// color #1
	else
		$rcols=str_replace('<!--#SIZE#-->', $sm1, $lta['cols']);	// colors #1-<!--#SIZE#-->
	// Color category sections 1-<!--#SIZE#--> of the main menu toolbar created from <!--#COLS#--> incl. submenu toolbars.
	$a['dsc']=strtr($lta['carea'], array('<!--#SIZE#-->'=>$ms, '<!--#COLS#-->'=>$rcols));

	// col: Color fields
	$tw=$cw-($sm1*16);
	if($sm1 < $ms)
		$tw -= (16+21);
	$tw=floor($tw/($sm1+1)) - 5;
//echo('$cw='.$cw.', $sm1='.$sm1.' &rarr; $tw='.$tw.'<hr />');
	$a['col']  = '<table width="100%" border="0" cellpadding="0" cellspacing="0"><tr><td width="'.$cw.'" align="right" nowrap><div style="position:relative;margin:0;padding:0;">';
	// ...Standard menu background
	$a['col'] .= '<div style="position:absolute;left:0;top:0;">';
	$a['col'] .= '<input class="fxftx" type="color" id="mset_cmenu1-i" name="mset_cmenu1-i" tabindex="0" fxform="_fxform_datmask_3" value="'.formatHTMLColor($m0col, true).'" tooltip="'.$dsa['cmenu1-i']['name'].' &rarr; '.$lta['chg'].': <b>'.$m0col.'</b>" style="width:'.$tw.'px;padding:2px;margin:0;border:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';cursor:pointer;">';
	$a['col'] .= '<div class="darkergrey" style="position:absolute;left:0;top:-12px;width:'.$tw.'px;font-size:10px;text-align:center;">BG</div>';
	$a['col'] .= '<div class="grey" style="position:absolute;right:-12px;top:4px;padding:2px;">|</div>';
	$a['col'] .= '</div>';
	$trc=0;
	foreach($m1col as $skey => $svalue)
	{
		if($skey)
			$a['col'] .= '<div class="grey" style="position:relative;top:-4px;padding:2px;text-align:center;display:inline-block;">&rarr;</div>';
		$a['col'] .= '<div style="position:relative;display:inline-block;">';
		$a['col'] .= '<input class="fxftx" type="color" id="mset_cmenu-c'.($skey+1).'" name="mset_cmenu-c'.($skey+1).'" tabindex="0" fxform="_fxform_datmask_3" value="'.formatHTMLColor($svalue, true).'" tooltip="'.$dsa['cmenu-c'.($skey+1)]['name'].' &rarr; '.$lta['chg'].': <b>'.$svalue.'</b>" style="width:'.$tw.'px;padding:2px;margin:0;border:0;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';cursor:pointer;">';
		$a['col'] .= '<div class="darkergrey" style="position:absolute;left:0;top:-12px;width:'.$tw.'px;font-size:10px;text-align:center;">#'.($skey+1).'</div>';
		if($skey)
			$a['col'] .= '<img src="'.$GLOBALS['gfxpath'].'del_c_80x80.png" tooltip="'.$dsa['cmenu-c'.($skey+1)]['name'].' &rarr; '.$lta['cdel'].': <b>'.$svalue.'</b>" style="position:absolute;top:-7px;right:-8px;height:18px;background:#fcfcfc;border:1px solid #fff;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';border-radius:10px;cursor:pointer;" '.fxf_jsFunction('designField', 'mset_cmenu-c'.($skey+1), 'del', 'event').'>';
		$a['col'] .= '</div>';
	}
	// Add new category section color?
	if($sm1 < $ms)
	{
		$a['col'] .= '<div class="grey" style="position:relative;top:-4px;padding:2px;text-align:center;display:inline-block;">&rarr;</div>';
		$a['col'] .= '<div tooltip="'.$lta['cadd'].' &rarr; #'.($skey+2).'" style="position:relative;top:-2px;width:20px;height:27px;margin:0;text-align:center;font-size:19px;font-weight:bolder;color:#009f6b;background:#fcfcfc;border-right:1px solid #'.$GLOBALS['msk_nbcol'].';border-bottom:1px solid #'.$GLOBALS['msk_nbcol'].';cursor:pointer;display:inline-block;" '.fxf_jsFunction('designField', 'mset_cmenu-c'.($skey+2), $svalue, 'event').'>+</div>';
	}
	$a['col'] .= '</td></tr></table>';
//echo('<pre>'.fxHtmlEncode($a['col']).'</pre>');

	// preview: Menu sections
	$cw=floor(($ew-$ms-1)/$ms);	//  70
	if($cw%2)
		$cw--;
	$iw=($cw+1)*$ms-1;	// 638
	$ih=5*30+4;	// 154
	$pl=ceil(($ew-$iw)/2);	//   1
//echo('$ew='.$ew.', $ms='.$ms.' -- $cw='.$cw.' -- $iw='.$iw.', $ih='.$ih.' -- $pl='.$pl.'<hr />');
	$a['prv']  = '<br /><br />';
	$a['prv'] .= '<div style="position:relative;left:'.$pl.'px;width:'.$iw.'px;height:'.$ih.'px;border:1px solid #000000;">'.$GLOBALS['nl'];
	// ...Menu bar
	$w0=$cw/2-1;
	$wa=0;
	$a['prv'] .= '	<div class="dp_cmenu1-i" style="float:left;position:relative;width:'.($w0).'px;height:30px;border-right:1px solid #000000;border-bottom:1px solid #000000;"></div>'.$GLOBALS['nl'];
	foreach($mca[1] as $m1 => $m1a)
	{
		// ...Skip 0=Sysadmin
		if($m1 && ($m1 < $ms))
		{
			$wd=round(($iw-$cw-2)/($ms-1));
			$wa += $wd;

			$scol=$m1a[0]['background'];
			$ecol=$mca[1][$m1+1][0]['background'];

			$a['prv'] .= '	<div style="float:left;position:relative;width:'.($wd).'px;height:30px;border-bottom:1px solid #000000;background:-webkit-linear-gradient(left,'.$scol.','.$ecol.');"></div>'.$GLOBALS['nl'];
		}
	}
	$a['prv'] .= '	<div class="dp_cmenu1-i" style="float:left;position:relative;width:'.($iw-$w0-$wa-2).'px;height:30px;border-left:1px solid #000000;border-bottom:1px solid #000000;"></div>'.$GLOBALS['nl'];
	// ...Menu color positions
	$a['prv'] .= '	<div style="position:absolute;left:'.($w0+1).'px;top:0;width:'.($iw-$cw).'px;height:30px;">'.$GLOBALS['nl'];
	foreach($m1col as $m1 => $col)
	{
		if($sm1 == 1)
			$cp='left:50%;';
		else if(!$m1)
			$cp='left:-1px;';
		else if($m1 == $sm1-1)
			$cp='right:-2px;';
		else
			$cp='left:'.round($m1*100/($sm1-1)).'%;';
//echo('$m1='.$m1.', $cp='.$cp.'<br />');
		$a['prv'] .= '		<div style="position:absolute;top:-6px;'.$cp.'width:1px;height:12px;border-left:1px solid #000000;"><div style="position:absolute;left:-8px;top:-16px;font-weight:bold;">#'.($m1+1).'</div></div>'.$GLOBALS['nl'];
	}
	$a['prv'] .= '	</div>'.$GLOBALS['nl'];
	// ...Menu sections
	foreach($mca[1] as $m1 => $m1a)
	{
		// ...Skip 0=Sysadmin
		if($m1)
		{
			$m2=substr('0'.$m1,-2);
			$cb='';
			if($m1 == 1)
				$cb='clear:both;';
			$wa=-1;
			if($m1 == $ms)
				$wa=1;
			$bd='';
			if($m1 > 1)
				$bd='border-left:1px solid #000000;';
			$a['prv'] .= '<div style="'.$cb.'float:left;position:relative;width:'.$cw.'px;height:123px;'.$bd.'">'.$GLOBALS['nl'];
			$a['prv'] .= '	<div class="dp_cmenu1-'.$m2.'a" style="position:absolute;left:0;top:0;width:100%;height:30px;border-bottom:1px solid #000000;cursor:default;"><span style="line-height:30px;">#'.$m1.'.1</span></div>'.$GLOBALS['nl'];
			$a['prv'] .= '	<div class="dp_cmenu2-'.$m2.'i dp_cmenu2-'.$m2.'" style="position:absolute;left:0;top:31px;width:100%;height:30px;border-bottom:1px solid #000000;cursor:default;"><span style="line-height:30px;">#'.$m1.'.2</span></div>'.$GLOBALS['nl'];
			$a['prv'] .= '	<div class="dp_cmenu3-'.$m2.'i dp_cmenu3-'.$m2.'" style="position:absolute;left:0;top:62px;width:100%;height:30px;border-bottom:1px solid #000000;cursor:default;"><span style="line-height:30px;">#'.$m1.'.3</span></div>'.$GLOBALS['nl'];
			$a['prv'] .= '	<div class="dp_cmenu3-'.$m2.'a" style="position:absolute;left:0;top:93px;width:100%;height:30px;cursor:default;"><span style="line-height:30px;">#'.$m1.'.4</span></div>'.$GLOBALS['nl'];
			$checked='';
			if($m1 == $msai)
				$checked=' checked';
			$a['prv'] .= '	<div style="position:absolute;top:-10px;left:'.($cw/2-12).'px;"><input class="fxfra" id="msec'.$m1.'" name="msec" type="radio" tabindex="0" value="'.$m1.'" tooltip="'.str_replace('<!--#CSN#-->', $m1, $lta['pdsp']).'" style="cursor:pointer;"'.$checked.'></div>'.$GLOBALS['nl'];
			$a['prv'] .= '</div>'.$GLOBALS['nl'];
		}
	}
	$a['prv'] .= '</div>';

	return $a;
}

/**
 * ???FUNCTION???
 *
 * @param various $dsa       - Mandatory parameter:                   ???PARAMETER???
 * @param string  $msa       - Optional parameter (default = '00'):   ???PARAMETER???
 * @param boolean $only_menu - Optional parameter (default = false):  ???PARAMETER???
 *
 * @return ???RETURN???
 */
function getDesignPreview($dsa, $msa='00', $only_menu=false)
{
	// Display function call?
	if($GLOBALS['__debug']['debugmode']&FXP_DEBUG_FCT) { fxDebugFunction(); }

	$p='';

	$msai=(int)$msa;
	$msa=substr('0'.$msai,-2);

	$ew=640;
	if(function_exists('db_value'))
		$ew=(int)db_value("SELECT breite FROM masken WHERE nummer=15 and mandanten_id=0");

	$pw=max(498,$GLOBALS['fxpglobals']['swidth']-$ew-140);
	$ph=$pw*0.5625;
	$hh=round($ph/30);
	$nh=$hh*2;

	$ms=0;
	$m0col=$dsa['cmenu1-i']['background'];
	$m1col=array();
	if(fxIsArray($dsa))
	{
		while(fxIsArray($dsa['cmenu-c'.($ms+1)]))
		{
			if(isset($dsa['cmenu-c'.($ms+1)]['background']) && strlen((string)$dsa['cmenu-c'.($ms+1)]['background']) && ($dsa['cmenu-c'.($ms+1)]['background'] != 'transparent'))
				$m1col[]=$dsa['cmenu-c'.($ms+1)]['background'];
			$ms++;
		}
	}
//fxDebug($m1col,'$m1col', 0);
	$mca=getMenuColors($ms, $m0col, $m1col);
//fxDebug($mca,'$mca', 0);

	$mouseoo=' onmouseover="$(this).style.boxShadow=\'0 0 6px #000 inset\';$(this).style.cursor=\'pointer\';" onmouseout="$(this).style.boxShadow=\'\';$(this).style.cursor=\'default\';"';
	$mouseooc=$mouseoo.' '.fxf_jsFunction('designField', '^^event.stopPropagation();', 'Feld', '$(this).id');

	// ...Background (START)
	if(!$only_menu)
	{
		$p .= '<div style="position:realtive;border:1px solid rgba(0,0,0, 0.5);box-shadow:4px 4px 4px rgba(0,0,0, 0.25);">'.$GLOBALS['nl'];
		$p .= '	<div id="dp_cfxbody" class="dp_cfxbody" tooltip="'.$dsa['cfxbody']['name'].'" style="cursor:pointer;position:relative;left:0;top:0;width:'.$pw.'px;height:'.$ph.'px;overflow:hidden;"'.$mouseooc.'>'.$GLOBALS['nl'];
		// ...Navigation toolbar
		$p .= '		<div id="dp_fxtool" style="position:absolute;left:0;top:0;right:0;height:'.$nh.'px;">'.$GLOBALS['nl'];
		$p .= '			<div id="dp_ifxtoolc" tooltip="'.$dsa['ifxtoolc']['name'].'" style="cursor:pointer;position:absolute;left:0;top:0;right:0;bottom:0;float:right;"'.$mouseooc.'>'.$GLOBALS['nl'];
		$p .= '				<img src="data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAAFkAAAAvCAYAAACIcGZzAAABA0lEQVR4nO3QsQ1CQRAD0emJNqiDNmidgPATILizfdauNPn6ATyn5T2AO++7EfBQWxfgQRYAD7IAeJAFwIMsAB5kAfAgC4AHWQA8yALgQRYAD7IAeJAFwAQMSO9vYAJGJLcEmIAhqS0DJmBMYkuBCRiU1nJgAkYltQWYgGEpbQMmYFxCW4EJGOhuOzABI+uBCRhaD4xhXEJSYASD0pIDs2FEchZgFjx+SjZgfnj2xKzAfPHg6dmB+fBUUxHA4IeoBwY/Rj0w+EHqgcGPUg8Mfph6YPDj1AODH6geGPxI9cDgh6oHBj9WPTD4weqBwY9WDwx+uHpg8OPVA4MfsB4Y/Ij1wC+9Mc/1UdaqWQAAAABJRU5ErkJggg==" style="position:absolute;left:0;top:0;height:'.$nh.'px;">'.$GLOBALS['nl'];
		$p .= '				<img src="'.fxGetLogo('small').'" style="position:absolute;left:2px;top:2px;height:'.($nh-4).'px;">'.$GLOBALS['nl'];
		$p .= '				<span style="position:absolute;right:1px;top:11px;float:right;">'.$GLOBALS['nl'];
		// ...Toolbar: Search
		$p .= '					<div class="ticon" style="position:relative;top:-3px;" tooltip="'.$dsa['ifxsrcinput']['name'].'"><input id="dp_ifxsrcinput" class="srccls" type="text" value="Search" style="float:left;width:80px;padding:2px 4px;padding-top:1px;color:#949494;border:1px solid rgba(0,0,0, 0.8);border-bottom:1px solid rgba(255,255,255, 0.8);border-right:0;font-size:10px;display:inline-block;cursor:pointer;"'.$mouseooc.'><div class="icon13" style="top:-1px;margin-left:-2px;margin-right:9px;"><img class="icon13p icontbr" src="'.$GLOBALS['gfxpath'].'src_w_13x13.png"></div></div>'.$GLOBALS['nl'];
		// ...Toolbar: Nav-Icons
		for($ni=3; $ni>0; --$ni)
			$p .= '					<div id="dp_cticontxt" class="ticon" style="position:relative;top:-2px;" tooltip="'.$dsa['cticontxt']['name'].'"'.$mouseooc.'><div class="icon13" style="margin-right:4px;"><img class="icon13p" src="'.$GLOBALS['gfxpath'].'inf_w_13x13.png" style="top:3px;"></div><span class="dp_cticontxt" style="font-size:10px;">&nbsp;Nav-Link-'.$ni.'</span></div>'.$GLOBALS['nl'];
		$p .= '			</div>'.$GLOBALS['nl'];
		$p .= '		</div>'.$GLOBALS['nl'];

		// ...Menu section
		$p .= '		<div id="dsg_menusection" tooltip="Menu" style="cursor:help;" onclick="event.stopPropagation();return;">';
	}

	// ...Menu
	$btc=$mca[1][$msai][0]['background'];
	$p .= '			<div style="position:relative;left:0;top:'.$nh.'px;width:100%;margin:0;padding:0;border-top:3px solid '.$btc.';box-shadow:0 2px 2px rgba(0,0,0, 0.25);z-index:140;"><div class="dp_cmenu1-i" style="overflow:hidden;">';
	for($m=1; $m<=$ms; $m++)
	{
		$m2=substr('0'.$m,-2);
		$ca='';
		if($m2 == $msa)
			$ca='a';
		$p .= '<div class="dp_cmenu1-'.$m2.$ca.'" style="float:left;padding:'.(($hh-13)/2).'px 4px;font-size:10px;">Menu #'.$m.'.1</div>';
	}
	$p .= '</div></div>'.$GLOBALS['nl'];
	$p .= '			<div style="position:relative;left:0;top:'.($nh-1).'px;width:100%;margin:0;padding:0;box-shadow:0 2px 2px rgba(0,0,0, 0.25);z-index:139;"><div class="dp_cmenu2-'.$msa.'i" style="overflow:hidden;">';
	for($m=1; $m<=($ms+1); $m++)
	{
		$ca='';
		$cn='2';
		if($m == $msai+1)
		{
			$ca='a';
			$cn='3';
		}
		$p .= '<div class="dp_cmenu2-'.$msa.$ca.'" style="float:left;padding:'.(($hh-13)/2).'px 4px;font-size:10px;">Menu #'.$msai.'.'.$cn.'</div>';
	}
	$p .= '</div></div>'.$GLOBALS['nl'];
	$p .= '			<div style="position:relative;left:0;top:'.($nh-2).'px;width:100%;margin:0;padding:0;box-shadow:0 2px 2px rgba(0,0,0, 0.25);z-index:138;"><div class="dp_cmenu3-'.$msa.'i" style="overflow:hidden;">';
	for($m=1; $m<=($ms+2); $m++)
	{
		$ca='';
		$cn='3';
		if($m == $msai+2)
		{
			$ca='a';
			$cn='4';
		}
		$p .= '<div class="dp_cmenu3-'.$msa.$ca.'" style="float:left;padding:'.(($hh-13)/2).'px 4px;font-size:10px;">Menu #'.$msai.'.'.$cn.'</div>';
	}
	$p .= '</div></div>'.$GLOBALS['nl'];

	if(!$only_menu)
	{
		$action_style='float:left;width:120px;margin-left:6px;padding:3px 0;font-size:10px;text-align:center;border:1px solid rgba(0,0,0, 0.33); border-radius:8px;';
		$button_style='padding:1px 16px 2px 16px;border-radius:8px;font-size:14px;font-weight:bold;text-shadow:1px 1px 1px rgba(0,0,0, 0.75);box-shadow:5px 5px 5px rgba(255,255,255, 0.5) inset, -5px -5px 5px rgba(0,0,0, 0.5) inset, 4px 4px 4px rgba(0,0,0, 0.5);';
		$workf_style='border:1px solid rgba(0,0,0, 0.25);border-bottom:0;';
		$maskc_style='margin:2px 8px 8px 2px;border-radius:10px;box-shadow:4px 4px 8px rgba(0,0,0, 0.25);';
		$maskh_style='float:none;padding:8px;border:0;border-top-left-radius:8px;border-top-right-radius:8px;';

		$p .= '		</div>'.$GLOBALS['nl'];

		// ...Main
		$p .= '		<div id="dp_fxmain" style="position:absolute;left:0;top:'.($hh*6).'px;right:0;bottom:'.($hh-2).'px;">'.$GLOBALS['nl'];
		// ......Frame
		$p .= '			<div id="dp_fxframe" style="position:absolute;left:4px;top:0;right:4px;bottom:4px;">'.$GLOBALS['nl'];
		// ......Frame: Path
		$p .= '				<div id="dp_cttitle" class="dp_cttitle" tooltip="'.$dsa['cttitle']['name'].'" style="position:absolute;left:2px;top:-2px;padding:3px;;font-size:10px;"'.$mouseooc.'>'.$dsa['cttitle']['name'].'</div>'.$GLOBALS['nl'];
		// ......Frame: Action
		$p .= '				<div class="nousersel" style="position:absolute;right:2px;top:-2px;white-space:nowrap;">'.$GLOBALS['nl'];
		$p .= '					<span id="dp_cactreg" class="dp_cactreg" tooltip="'.$dsa['cactreg']['name'].'" style="'.$action_style.'border-style:solid;font-weight:bolder;text-shadow:1px 1px 1px rgba(0,0,0, 0.75);box-shadow:2px 2px 4px rgba(255,255,255, 0.75) inset;"'.$mouseooc.'>'.$dsa['cactreg']['name'].'</span>'.$GLOBALS['nl'];
		$p .= '					<span id="dp_cactrega" class="dp_cactrega" tooltip="'.$dsa['cactrega']['name'].'" style="'.$action_style.'border-style:solid;font-weight:bolder;text-shadow:1px 1px 1px rgba(0,0,0, 0.75);box-shadow:2px 2px 4px rgba(0,0,0, 0.75) inset;"'.$mouseooc.'>'.$dsa['cactrega']['name'].'</span>'.$GLOBALS['nl'];
		$p .= '					<span id="dp_cactregi" class="dp_cactregi" tooltip="'.$dsa['cactregi']['name'].'" style="'.$action_style.'border-style:dotted;opacity:0.5;"'.$mouseooc.'>'.$dsa['cactregi']['name'].'</span>'.$GLOBALS['nl'];
		$p .= '				</div>'.$GLOBALS['nl'];
		// .........Workframe
		$p .= '				<div id="dp_fxworkframe" style="position:absolute;left:1px;top:'.round($hh*1.1).'px;right:3px;bottom:0;border-radius:5px;box-shadow:3px 3px 3px rgba(0,0,0, 0.5);">'.$GLOBALS['nl'];
		// .........Workframe: Header
		$p .= '					<div id="dp_ifxheaderc" tooltip="'.$dsa['ifxheaderc']['name'].'" style="position:absolute;left:0;top:0;right:0;height:'.round($hh*1.5).'px;border-top-left-radius:5px; border-top-right-radius:5px;overflow:hidden;"'.$mouseooc.'>'.$GLOBALS['nl'];
		$p .= '						<div id="dp_ifxheader" style="position:absolute;left:0;top:0;right:0;bottom:0;font-size:14px;font-weight:bold;text-shadow:2px 2px 2px rgba(0,0,0, 0.5);padding:6px 9px;">'.$dsa['ifxheaderc']['name'].'</div>'.$GLOBALS['nl'];
		// ............Workframe: Header-Buttons
		$p .= '						<div id="dp_cfxfbs" class="dp_cfxfbs" tooltip="'.$dsa['cfxfbs']['name'].'" style="'.$button_style.'position:absolute;right:24px;top:4px;"'.$mouseooc.'>'.$dsa['cfxfbs']['name'].'</div>'.$GLOBALS['nl'];
		$p .= '					</div>'.$GLOBALS['nl'];
		// .........Workframe: Work area
		$p .= '					<div id="dp_ifxworkf" class="dp_cfxworkf" style="'.$workf_style.'position:absolute;left:0;top:'.round($hh*1.5).'px;right:0;bottom:10px;overflow:hidden;">'.$GLOBALS['nl'];
		$p .= '						<div id="dp_ifxwork" tooltip="'.$dsa['ifxwork']['name'].'" style="position:absolute;left:2px;top:2px;right:2px;bottom:2px;padding:6px;overflow-x:auto;overflow-y:scroll;"'.$mouseooc.'>'.$dsa['ifxwork']['name'].$GLOBALS['nl'];
		// ............Mask
		$p .= '							<br /><br />'.$GLOBALS['nl'];
		$p .= '							<div id="dp_cmaskc" class="dp_cmaskc" tooltip="'.$dsa['cmaskc']['name'].'" style="'.$maskc_style.'position:relative;width:'.($pw/2).'px;white-space:normal;cursor:help;"'.$mouseooc.'>'.$GLOBALS['nl'];
		$p .= '								<div id="dp_cmaskh" class="dp_cmaskh" tooltip="'.$dsa['cmaskh']['name'].'" style="'.$maskh_style.'"'.$mouseooc.'><span class="maskht" style="padding:3px;font-size:14px;">'.$dsa['cmaskh']['name'].'</span></div>'.$GLOBALS['nl'];
		$p .= '								<div class="maskm" style="white-space:normal;">'.$dsa['cmaskc']['name'].'<br /><br /><br /><br /></div>'.$GLOBALS['nl'];
		$p .= '								<div class="maskb" style="position:relative;display:block;">'.$GLOBALS['nl'];
		// ...............Mask-Button
		$p .= '									<span id="dp_cfxfbs" class="dp_cfxfbs" tooltip="'.$dsa['cfxfbs']['name'].'" style="'.$button_style.'"'.$mouseooc.'>'.$dsa['cfxfbs']['name'].'</span>'.$GLOBALS['nl'];
		$p .= '								</div>'.$GLOBALS['nl'];
		$p .= '							</div>'.$GLOBALS['nl'];
		$p .= '						</div>'.$GLOBALS['nl'];
		$p .= '					</div>'.$GLOBALS['nl'];
		// .........Workframe: Footer
		$p .= '					<div id="dp_ifxfooterc" tooltip="'.$dsa['ifxfooterc']['name'].'" style="position:absolute;left:0;right:0;bottom:0;height:10px;border-bottom-left-radius:5px;border-bottom-right-radius:5px;overflow:hidden;"'.$mouseooc.'>'.$GLOBALS['nl'];
		$p .= '						<div id="dp_ifxfooter" style="position:absolute;left:6px;top:0;right:6px;bottom:0;font-size:8px;">'.$dsa['ifxfooterc']['name'].'<div style="position:absolute;right:0;top:0;font-size:8px;color:rgba(255,255,255, 0.25);">Copyright</div></div>'.$GLOBALS['nl'];
		$p .= '					</div>'.$GLOBALS['nl'];
		$p .= '				</div>'.$GLOBALS['nl'];
		$p .= '			</div>'.$GLOBALS['nl'];
		$p .= '		</div>'.$GLOBALS['nl'];

		// ...Taskbar
		$p .= '		<div id="dp_ifxtask" tooltip="'.$dsa['ifxtask']['name'].'" style="position:absolute;left:0;bottom:0;right:0;height:'.($hh-3).'px;font-size:10px;line-height:'.($hh-4).'px;"'.$mouseooc.'>&nbsp;'.$dsa['ifxtask']['name'].'</div>'.$GLOBALS['nl'];
	}

	// ...Background (END)
	if(!$only_menu)
	{
		$p .= '	</div>'.$GLOBALS['nl'];
		$p .= '</div>'.$GLOBALS['nl'];
	}

	return $p;
}
?>