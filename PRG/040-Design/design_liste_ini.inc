<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : design_liste_ini.inc                                         //
// Version     : 21.1                                                         //
// Begin       : 2021-02-01                                                   //
// Last Change : 2021-02-01                                                   //
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
 * PF 40: Design
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$tcss_filename=fxSessionFile('t40-temp.css');
$mode=FXP_DISPLAY;
$dsg='';
$msa='01';
if(fxIsArray($_POST) && isset($_POST['Button_Speichern']))
{
	$mode=(int)$_POST['mode'];
	$dsg=trim($_POST['dsg']);
	$msa=trim($_POST['msa']);
}
else if(fxIsArray($_GET) && isset($_GET['d']))
{
	$dsg=trim($_GET['d']);
	if(fxIsArray($_POST))
	{
		if(isset($_POST['dcreate']))
			$mode=FXP_CREATE;
		else if(isset($_POST['dedit']))
			$mode=FXP_CHANGE;
		else if(isset($_POST['ddelete']))
			$mode=FXP_DELETE;
	}
}
$msai=(int)$msa;
//echo('$mode='.$mode.', $dsg='.$dsg.', $msa='.$msa.' (int='.$msai.')<hr />');

// Menu sections (without Sysadmin)
$ms=0;
$ma=loadMenu(true);
foreach($ma as $id => $sa)
{
	if(!$sa['lv'] && ($sa['tx'] != 'Sysadmin'))
		$ms++;
}
echo('<div id="ms" style="display:none;">'.$ms.'</div>');

$changed=false;
if($mode == FXP_DELETE)
{
	// All styles
	$fxdsa=fxpGetDesigns(true);

	// Delete selected style file(s)
	$cpl=1+strlen($GLOBALS['fxpglobals']['client']);
	$dname=substr($dsg,$cpl);
	$dfile=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/'.$dname.'.css';
	$pfile=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/'.$dname.'.png';
//echo('$dfile='.$dfile.'<br />$pfile='.$pfile.'<br />');
	if(file_exists($dfile))
		unlink($dfile);
	if(file_exists($pfile))
		unlink($pfile);

	// Success message
	$ata=explode(',', meldung(14));	// displayed,created,changed,deleted
	error_msg(3036,false,$fxdsa[$dsg],$ata[$mode-1]);	// The design "$var1" was successfully $var2.

	$changed=true;
}
else if(isset($_POST['Button_Speichern']) && (($mode == FXP_CREATE) || ($mode == FXP_CHANGE)))
{
	// Name
	if(!strlen($_POST['design']))
	{
		error_msg(749,false,'design');	// 749: Das Feld "$var1" darf nicht leer sein. Bitte geben Sie einen Text ein!
		if(fxIsArray($GLOBALS['fxpvars']['lookups'][10]))
		{
			$GLOBALS['fxpvars']['lookups'][15]=$GLOBALS['fxpvars']['lookups'][10];
			unset($GLOBALS['fxpvars']['lookups'][10]);
		}
		$err=true;
	}
	else
	{
		$tcss=fxLoad($tcss_filename);
		$dsa=fxpClientDesign($tcss,$ms);
//fxDebug($dsa,'$dsa: $tcss_filename='.$tcss_filename, 0);
		$tcss=fxpCreateClientCSS($_POST['design'],$dsa,$ms,false);

		if($mode == FXP_CHANGE)
		{
			$cpl=1+strlen($GLOBALS['fxpglobals']['client']);
			$dname=substr($dsg,$cpl);
			$dfile=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/'.$dname.'.css';
		}
		else
		{
			$dnl=strtolower(trim($_POST['design']));
			$dname='';
			for($i=0; $i<strlen($dnl); $i++)
			{
				$dc=substr($dnl,$i,1);
				if(ord($dc) == 0x20)	// Space
					break;
				if((ord($dc) >= 0x61) && (ord($dc) <= 0x7a))	// Only a-z are valid
					$dname .= $dc;
			}
			$sdn=$dname;
			$sdc=1;
			while(true)
			{
				$dfile=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/'.$dname.'.css';
				if(!file_exists($dfile))
					break;
				$dname=$sdn.$sdc;
				$sc++;
			}
		}
//echo('$dname='.$dname.', $dfile='.$dfile.'<br />');

		// Save client design file
		$tcss=strtr($tcss, array('dp_h'=>'', 'dp_i'=>'', 'dp_c'=>''));
//echo('<b>$tcss</b><pre>'.fxHtmlEncode($tcss).'</pre>');
		fxSave($dfile,$tcss,true);

		// Create preview icon
		$pfile=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/'.$dname.'.png';
//echo('$pfile='.$pfile.'<br />');
		if(function_exists('imagecreatetruecolor'))
		{
$drawdebug=false;
			// Create PNG
			$im=imagecreatetruecolor(30,18);

			// ...Background
			$o=0;
			$cst=$dsa['cfxbody']['background*'];
			if(substr($cst,1,1) == '|')
				$cst=substr($cst,2);
			$car=getInterpolatedHTMLColorArray($cst,47);
if($drawdebug) echo('Start drawing [Background]: $o='.$o.'<br />');
			foreach($car as $cco => $ccl)
			{
				$col=imagecolorallocate($im, hexdec(substr($ccl,0,2)), hexdec(substr($ccl,2,2)), hexdec(substr($ccl,4)));

				$px1=min(29, max(0, $o-17));
				$py1=min(17, max(0, $o));
				$px2=min(29, max(0, $o));
				$py2=min(17, max(0, $o-29));
if($drawdebug) echo('&middot; Draw line '.$o.' with color #'.$ccl.': '.$px1.','.$py1.' - '.$px2.','.$py2.'<br />');
				imageline($im, $px1,$py1, $px2,$py2, $col);
				$o++;
			}
if($drawdebug) echo('End drawing: $o='.$o.'<hr />');

			// ...Navigation Toolbar
			$o=0;
			$cst=$dsa['ifxtoolc']['background*'];
			if(substr($cst,1,1) == '|')
				$cst=substr($cst,2);
			$car=getInterpolatedHTMLColorArray($cst,12);
if($drawdebug) echo('Start drawing [Navigation Toolbar]: $o='.$o.'<br />');
			foreach($car as $cco => $ccl)
			{
				$col=imagecolorallocate($im, hexdec(substr($ccl,0,2)), hexdec(substr($ccl,2,2)), hexdec(substr($ccl,4)));

				$px1=min(29, max(0, $o-17));
				$py1=min(17, max(0, $o));
				$px2=min(29, max(0, $o));
				$py2=min(17, max(0, $o-29));
if($drawdebug) echo('&middot; Draw line '.$o.' with color #'.$ccl.': '.$px1.','.$py1.' - '.$px2.','.$py2.'<br />');
				imageline($im, $px1,$py1, $px2,$py2, $col);
				$o++;
			}
if($drawdebug) echo('End drawing: $o='.$o.'<hr />');

			// ...Menu Color #1
if($drawdebug) echo('Start drawing [Menu Color #1]: $o='.$o.'<br />');
			$ccl=formatHTMLColor($dsa['cmenu-c1']['background']);
			$col=imagecolorallocate($im, hexdec(substr($ccl,0,2)), hexdec(substr($ccl,2,2)), hexdec(substr($ccl,4)));
			$px1=min(29, max(0, $o-18));
			$py1=min(17, max(0, $o));
			$px2=min(29, max(0, $o));
			$py2=min(17, max(0, $o-30));
if($drawdebug) echo('&middot; Draw line '.$o.' with color #'.$ccl.': '.$px1.','.$py1.' - '.$px2.','.$py2.'<br />');
			imageline($im, $px1,$py1, $px2,$py2, $col);
			$o++;
if($drawdebug) echo('End drawing: $o='.$o.'<hr />');

			// ...Menu Background
if($drawdebug) echo('Start drawing [Menu Background]: $o='.$o.'<br />');
			$ccl=formatHTMLColor($dsa['cmenu1-i']['background']);
			$col=imagecolorallocate($im, hexdec(substr($ccl,0,2)), hexdec(substr($ccl,2,2)), hexdec(substr($ccl,4)));
			for($c=0; $c<7; $c++)
			{
				$px1=min(29, max(0, $o-17));
				$py1=min(17, max(0, $o));
				$px2=min(29, max(0, $o));
				$py2=min(17, max(0, $o-29));
if($drawdebug) echo('&middot; Draw line '.$o.' with color #'.$ccl.': '.$px1.','.$py1.' - '.$px2.','.$py2.'<br />');
				imageline($im, $px1,$py1, $px2,$py2, $col);
				$o++;
			}
if($drawdebug) echo('End drawing: $o='.$o.'<hr />');

			// ...Work Area Header
			$o += 5;
			$cst=$dsa['ifxheaderc']['background*'];
			if(substr($cst,1,1) == '|')
				$cst=substr($cst,2);
			$car=getInterpolatedHTMLColorArray($cst,5);
if($drawdebug) echo('Start drawing [Work Area Header]: $o='.$o.'<br />');
			foreach($car as $cco => $ccl)
			{
				$col=imagecolorallocate($im, hexdec(substr($ccl,0,2)), hexdec(substr($ccl,2,2)), hexdec(substr($ccl,4)));

				$px1=min(29, max(0, $o-17));
				$py1=min(17, max(0, $o));
				$px2=min(29, max(0, $o));
				$py2=min(17, max(0, $o-29));
if($drawdebug) echo('&middot; Draw line '.$o.' with color #'.$ccl.': '.$px1.','.$py1.' - '.$px2.','.$py2.'<br />');
				imageline($im, $px1,$py1, $px2,$py2, $col);
				$o++;
			}
if($drawdebug) echo('End drawing: $o='.$o.'<hr />');

			// ...Work Area Main
			$cst=$dsa['ifxwork']['background*'];
			if(substr($cst,1,1) == '|')
				$cst=substr($cst,2);
			$car=getInterpolatedHTMLColorArray($cst,10);
if($drawdebug) echo('Start drawing [Work Area Main]: $o='.$o.'<br />');
			foreach($car as $cco => $ccl)
			{
				$col=imagecolorallocate($im, hexdec(substr($ccl,0,2)), hexdec(substr($ccl,2,2)), hexdec(substr($ccl,4)));

				$px1=min(29, max(0, $o-17));
				$py1=min(17, max(0, $o));
				$px2=min(29, max(0, $o));
				$py2=min(17, max(0, $o-29));
if($drawdebug) echo('&middot; Draw line '.$o.' with color #'.$ccl.': '.$px1.','.$py1.' - '.$px2.','.$py2.'<br />');
				imageline($im, $px1,$py1, $px2,$py2, $col);
				$o++;
			}
if($drawdebug) echo('End drawing: $o='.$o.'<hr />');

			// ...Work Area Foot
			$cst=$dsa['ifxfooterc']['background*'];
			if(substr($cst,1,1) == '|')
				$cst=substr($cst,2);
			$car=getInterpolatedHTMLColorArray($cst,2);
if($drawdebug) echo('Start drawing [Work Area Foot]: $o='.$o.'<br />');
			foreach($car as $cco => $ccl)
			{
				$col=imagecolorallocate($im, hexdec(substr($ccl,0,2)), hexdec(substr($ccl,2,2)), hexdec(substr($ccl,4)));

				$px1=min(29, max(0, $o-17));
				$py1=min(17, max(0, $o));
				$px2=min(29, max(0, $o));
				$py2=min(17, max(0, $o-29));
if($drawdebug) echo('&middot; Draw line '.$o.' with color #'.$ccl.': '.$px1.','.$py1.' - '.$px2.','.$py2.'<br />');
				imageline($im, $px1,$py1, $px2,$py2, $col);
				$o++;
			}
if($drawdebug) echo('End drawing: $o='.$o.'<hr />');

			// ...Taskbar
			$o++;
			$cst=$dsa['ifxtask']['background*'];
			if(substr($cst,1,1) == '|')
				$cst=substr($cst,2);
			$car=getInterpolatedHTMLColorArray($cst,47-$o);
if($drawdebug) echo('Start drawing [Taskbar]: $o='.$o.'<br />');
			foreach($car as $cco => $ccl)
			{
				$col=imagecolorallocate($im, hexdec(substr($ccl,0,2)), hexdec(substr($ccl,2,2)), hexdec(substr($ccl,4)));

				$px1=min(29, max(0, $o-17));
				$py1=min(17, max(0, $o));
				$px2=min(29, max(0, $o));
				$py2=min(17, max(0, $o-29));
if($drawdebug) echo('&middot; Draw line '.$o.' with color #'.$ccl.': '.$px1.','.$py1.' - '.$px2.','.$py2.'<br />');
				imageline($im, $px1,$py1, $px2,$py2, $col);
				$o++;
			}
if($drawdebug) echo('End drawing: $o='.$o.'<hr />');

			// ...Save PNG
			$sok=imagepng($im, $pfile);
			imagedestroy($im);
		}

		// Success message
		$ata=explode(',', meldung(14));	// displayed,created,changed,deleted
		error_msg(3036,false,$_POST['design'],$ata[$mode-1]);	// The design "$var1" was successfully $var2.

		// Switch to display mode?
		unset($_POST['Button_Speichern']);
		if($mode == FXP_CREATE)
		{
			$dsg=$GLOBALS['fxpglobals']['client'].'C'.$dname;
			$mode=FXP_DISPLAY;
		}

		$changed=true;
	}
}

// Set changed design
if($changed)
{
	tGetDesign();
	echo('<div id="ps_design" style="display:none;">'.$GLOBALS['ps_design'].'</div>');
	$fxdst=fxpGetDesigns();
	echo('<div id="tb_design" style="display:none;">'.$fxdst.'</div>');
}

// Integrate company logo (upload)
if($_POST['int_clogo'])
{
//fxDebug('_POST');
	// 935: You have not selected a file.
	if(!fxIsArray($_POST['clogo_upload']))
		error_msg(935);
	// 1607: Error #$var1 during uploading file "$var2"!
	else if($_POST['clogo_upload']['error'])
		error_msg(1607, false, '<b>'.$_POST['clogo_upload']['error'].'</b>', '<i class=blue>'.$_POST['clogo_upload']['name'].'</i>');
	// 3050: The uploaded file "$var1" is no $var2 file!
	else if($_POST['clogo_upload']['type'] != 'image/png')
		error_msg(3050, false, '<i class=blue>'.$_POST['clogo_upload']['name'].'</i>', '<b>PNG</b>');
	// 936: File transfer successful.
	else
	{
		$clogo_path=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/logo.png';
		if(file_exists($clogo_path))
			unlink($clogo_path);
		@copy($_POST['clogo_upload']['new_name'], $clogo_path);

		error_msg(936);
	}
}

// Remove company logo
if($_POST['rem_clogo'])
{
	$clogo_path=$GLOBALS['datpath'].$GLOBALS['fxpglobals']['client'].'/logo.png';
	if(file_exists($clogo_path))
		unlink($clogo_path);

	// 3051: The company logo was successfully removed
	error_msg(3051);
}

// List of all designs
$fxdsa=fxpGetDesigns(true);
//fxDebug($fxdsa,'$fxdsa', 0);

$show_list=false;
$show_edit=true;
if(($mode == FXP_DISPLAY) || ($mode == FXP_DELETE))
{
	$show_list=true;
	$show_edit=false;
}
?>