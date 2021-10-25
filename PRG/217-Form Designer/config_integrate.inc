<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : config_integrate.inc                                         //
// Version     : 21.2                                                         //
// Begin       : 2020-10-15                                                   //
// Last Change : 2020-10-15                                                   //
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
 * PF 217: Form Designer - Integrate
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

// Search for and include "basics.inc" to set all definitions, variables and necessary dynamic paths
$__pnm='basics.inc';
if(!isset($GLOBALS['__loaded_'.$__pnm]))
{
	$__prg=$__pnm;
	$__pfc=0;
	while(($__pfc < 10) && !file_exists($__prg))
	{
		$__pfc++;
		$__prg='../'.$__prg;
	}
	if(file_exists($__prg))
	{
		require($__prg);
	}
	else
		die('<div style="padding:8px;"><h1 style="padding:8px;color:red;background:#fff;border:2px solid red;box-shadow:4px 4px 4px rgba(0,0,0, 0.5);">fx-project - ERROR: Necessary file &quot;'.$__pnm.'&quot; not found!</h1></div>');
}

// Prüfen, ob alle benötigten GET-Variablen mitgegeben wurden
$ajax=false;
$ktyp=0;
$mordner='';
$delpic='';
if(fxIsArray($_GET))
{
	if(isset($_GET['ajax']))
		$ajax=true;

	if(isset($_GET['ktyp']))
		$ktyp=(int)$_GET['ktyp'];

	if(isset($_GET['mordner']))
		$mordner=(int)$_GET['mordner'];

	// Evtl. Grafik löschen
	if(isset($_GET['delpic']) && strlen($_GET['delpic']))
		$delpic=$_GET['delpic'];
}

// Dateien ermitteln
$files=array();
if(($ktyp > 0) && strlen($mordner))
{
	$basedir_arr=array(0=>$GLOBALS['datpath'].$mordner);
	if($mordner != '0')
		$basedir_arr[1]=$GLOBALS['datpath'].'0';

	if($ktyp == 2018)
		$prefix='email_';
	else
		$prefix='gfcfg_';

	foreach($basedir_arr as $basedir)
	{
		if(@is_dir($basedir))
		{
			$dh=opendir($basedir);
			if($dh)
			{
				while(($fname=readdir($dh)) !== false)
				{
					$filename=$basedir.'/'.$fname;
					$ftype=filetype($filename);
					$suffix=substr($fname, -4);

					if(($ftype == 'file') && (substr($fname, 0, 6) == $prefix) && (($suffix == '.gif') || ($suffix == '.jpg') || ($suffix == '.png')))
					{
						if(strlen($delpic) && ($fname == $delpic))
							unlink($filename);
						else
						{
							$ims=GetImageSize($filename);
							$files[$fname]=array('path'=>$basedir, 'suffix'=>$suffix, 'width'=>$ims[0], 'height'=>$ims[1]);
						}
					}
				}
			}
		}
	}
}

// Ergebistext
$integrate='';
if(!sizeof($files))
	$integrate .= '<font class="blue">'.meldung(2041, false).'</font>';	// <b>Suche erfolgreich.</b> Es wurden jedoch noch keine Bilder hochgeladen.
else
{
//fxDebug($files, '$files');
	$text_delete_picture=meldung(2042, false);	// Bild löschen

	$text_tooltips=explode('|', meldung(2023, false));	// Bild hochladen|Bild einbinden|Kalkulator|Farbe|Bild|Text|Box|Rechteck|Linie

	$pstyle=' style="background:#cccccc;border:1px solid #bbbbbb;border-radius:4px;box-shadow:1px 1px 2px #666666;padding:4px;margin:2px;"';

	$integrate .= '<table border="0" cellpadding="0" cellspacing="4">';
	while(sizeof($files))
	{
		$integrate .= '<tr>';
		$lcnt=0;
		foreach($files as $filename => $fileparam)
		{
			$lcnt++;
			if($lcnt > 10)
				break;

			if($ktyp == 2018)
				$disp='<b class="blue">'.$filename.'</b>';
			else
				$disp='<b class=blue>^P</b> <b>'.$filename.'</b> <font class=lightgrey>|</font> -1 <font class=lightgrey>|</font> 1 <font class=lightgrey>|</font> 1.00 <font class=lightgrey>|</font> right';

			if($fileparam['width'] < $fileparam['height'])
				$imgparam=' height="80"';
			else
				$imgparam=' width="80"';

			$delicon= '';
			if($fileparam['path'] == $GLOBALS['datpath'].$mordner)
				$delicon='<img id="delpic'.$lcnt.'" src="'.$GLOBALS['gfxpath'].'icde.png" tooltip="'.$text_delete_picture.'" style="position:absolute;top:-3px;right:-3px;cursor:pointer;" '.fxf_jsFunction('configDeletePic', $filename);

			$inticon='<img id="intpic'.$lcnt.'" src="'.$GLOBALS['gfxpath'].'icad.png" tooltip="'.$text_tooltips[1].'" style="position:absolute;bottom:-3px;left:-3px;cursor:pointer;" onclick="$(\'conf_status_div\').innerHTML=\''.$disp.'\';">';

			$integrate .= '<td id="confpictd'.$lcnt.'" align="center" valign="middle" tooltip="'.$filename.' ('.$fileparam['width'].'x'.$fileparam['height'].')"'.$pstyle.'><div style="position:relative;width:80px;height:80px;">'.$inticon.$delicon.'<img src="'.$fileparam['path'].'/'.$filename.'" border="0"'.$imgparam.'></div></td>';
		}
		$integrate .= '</tr><tr>';
		$lcnt=0;
		foreach($files as $filename => $fileparam)
		{
			$lcnt++;
			if($lcnt > 10)
				break;

			if($ktyp == 2018)
				$dim=$fileparam['width'].'&nbsp;x&nbsp;'.$fileparam['height'];
			else
				$dim=round($fileparam['width']*0.65, 2).'&nbsp;x&nbsp;'.round($fileparam['height']*0.65, 2);

			$integrate .= '<td align="center" valign="middle" style="border:1px solid #cccccc;">&nbsp;<font class="s1">'.$dim.'</font>&nbsp;</td>';

			unset($files[$filename]);
		}
		$integrate .= '</tr>';
	}
	$integrate .= '</table>';
}

if($ajax)
	echo($integrate);
?>