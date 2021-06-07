<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : 231_ini.inc                                                  //
// Version     : 21.1                                                         //
// Begin       : 2021-02-18                                                   //
// Last Change : 2021-02-18                                                   //
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
 * PF 231: Manual
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

$brdcol='#dadada';
deleteClientMenuFiles(true, true);

// Dimensions
$dim_menu=420;

// Languages
$la=fxGetAllLanguages();
//fxDebug($la,'$la', 0);

// Load authorized/all program functions
$vt=array();
$st=array();
$ma=loadMenu();
if(fxIsArray($ma))
{
	foreach($ma as $tk => $tc)
	{
		if($tc['tr'])
		{
			$vt[$tc['tr']]=true;
			if(strlen($tc['pb']))
				$st[substr('00'.$tc['pb'],-2)]=(int)$tc['pb'];
		}
	}
}
if(sizeof($st))
	ksort($st);
//fxDebug($vt,'$vt', 0);
//fxDebug($st,'$st', 0);
$disp_all=strtolower(substr($GLOBALS['fxpglobals']['settings']['alle_tr_anzeig'],0,1));
//$disp_all='n';
if($disp_all !== 'n')
	$ma=loadMenu(true,false);
//fxDebug($ma,'$ma', 0);

// PDF's
$pa=array();
$pathname=$GLOBALS['docpath'].strtoupper($la[$GLOBALS['fxpglobals']['lang']]['tx']).'/';
$dir_handle=@opendir($pathname);
if($dir_handle)
{
	while(false !== ($file = @readdir($dir_handle)))
	{
		if(($file == '.') || ($file == '..'))
			continue;

		$filename=$pathname.$file;
		$prefix=substr($file, 0,10);
		$suffix=substr($file, -4);
		if(is_dir($filename) || ($prefix != 'fx-project') || ($suffix != '.pdf'))
			continue;

		$size=filesize($filename);
		$pa[$filename]=array('name'=>substr($file,11,-4), 'size'=>round($size/1048576,2), 'hr'=>false);
	}
	if(fxIsArray($pa))
		ksort($pa);
}
$pdf_disp='none';
if(fxIsArray($pa))
	$pdf_disp='inline';
//fxDebug($pa,'$pa', 0);

// Info
$vht=meldung(3023,false);	// Version History
$ia=array();
$cfa=array(
	'int'=>array('_intro.htm',meldung(524,false)),				// Allgemein
	'trl'=>array('_trans_extended.htm',meldung(42,false)),		// Programmfunktionen
	'hst'=>array('_history.htm',$vht)
);
if(sizeof($st))
{
	foreach($st as $stn => $sti)
		$cfa['h'.$stn]=array('_history_'.$stn.'.htm', $vht.' #'.$sti);
}
$cfa['wfl']=array('_workflow.htm', 'Workflow');
$cfa['man']=array('_menu.htm', meldung(2322,false));	// User Manual
foreach($cfa as $cfs => $cff)
{
	$filename=$GLOBALS['docpath'].$la[$GLOBALS['fxpglobals']['lang']]['no'].'/'.$cff[0];
	if(file_exists($filename))
	{
		$size=filesize($filename);
		$ia[$cfs]=array('name'=>$cff[1], 'size'=>round($size/1048576,2), 'tr'=>$cff[0], 'hr'=>false);
	}
}
$info_disp='none';
if(fxIsArray($ia))
	$info_disp='inline';
//fxDebug($ia,'$ia', 0);

$disp='inherit'; $overflow='hidden';
//$disp='none'; $overflow='auto';

echo('<div id="hb_container" style="position:absolute;left:0;right:0;top:12px;bottom:0;float:left;z-index:1;display:'.$disp.';">'.$nl);

echo('	<div style="position:absolute;left:-2px;top:0;width:'.$dim_menu.'px;height:28px;overflow:'.$overflow.';z-index:5;display:inline;">'.$nl);
echo('		<div id="hb_rtr" class="rega" '.fxf_jsFunction('changeReg', 'tr').'>'.$nl);
echo('			&nbsp;&nbsp;&nbsp;'.meldung(42,false).'&nbsp;&nbsp;&nbsp;'.$nl);	// Program Functions
echo('		</div>'.$nl);
echo('		<div id="hb_rin" class="regi" '.fxf_jsFunction('changeReg', 'in').' style="display:'.$info_disp.';">'.$nl);
echo('			&nbsp;&nbsp;&nbsp;'.meldung(1106,false).'&nbsp;&nbsp;&nbsp;'.$nl);	// Info
echo('		</div>'.$nl);
echo('		<div id="hb_rpd" class="regi" '.fxf_jsFunction('changeReg', 'pd').' style="display:'.$pdf_disp.';">'.$nl);
echo('			&nbsp;&nbsp;&nbsp;PDF\'s&nbsp;&nbsp;&nbsp;'.$nl);
echo('		</div>'.$nl);
echo('	</div>'.$nl);

echo('	<div style="position:absolute;left:0;top:27px;right:0;bottom:0;border:1px solid '.$brdcol.';background:#f4f4f4;overflow:'.$overflow.';z-index:2;display:inline;">'.$nl);

// ...Menu: Program Functions
echo('		<div id="hb_mtr" style="position:absolute;left:2px;top:2px;width:'.$dim_menu.'px;bottom:2px;line-height:22px;border:1px solid #dddddd;border-radius:4px;background:#ffffff;padding:4px;white-space:nowrap;overflow:auto;display:inline;">'.$nl);
if(fxIsArray($ma))
{
	$disp=false;
	foreach($ma as $tk => $tc)
	{
		$ma[$tk]['op']=0.33;
		if(!$disp && !$tc['lv'] && ($tc['tx'] != 'Sysadmin'))
			$disp=true;
		$ma[$tk]['disp']=$disp;

		if(isset($vt[$tc['tr']]))
		{
			$dtk=$tk;
			while(strlen($dtk) > 2)
			{
				$ma[$dtk]['disp']=true;
				if(!strlen($ma['eb']))
					$ma[$dtk]['op']=1.0;
				$dtk=substr($dtk,0,-3);
			}
		}
	}

	$c=0;
	foreach($ma as $tk => $tc)
	{
		if($tc['disp'])
		{
			if($c && !$tc['lv'])
				echo('			<br>'.$nl);
			if($tc['tr'])
				$str='<div class="favlink" style="margin-left:'.($tc['lv']*20).'px;margin-top:4px;opacity:'.$tc['op'].';cursor:pointer;" '.fxf_jsFunction('loadManual', $tc['tr'], '').'>'.$tc['tx'].'</div>';
			else
				$str='<div class="grey" style="margin-left:'.($tc['lv']*20).'px;margin-top:4px;text-decoration:underline;opacity:'.$tc['op'].';">'.$tc['tx'].'</div>';
			echo('			'.$str.$nl);
			$c++;
		}
	}
}
echo('			<br>'.$nl);
echo('		</div>'.$nl);

// ...Menu: PDF's
echo('		<div id="hb_mpd" style="position:absolute;left:2px;top:2px;width:'.$dim_menu.'px;bottom:2px;line-height:22px;border:1px solid #dddddd;border-radius:4px;background:#ffffff;padding:4px;white-space:nowrap;overflow:auto;display:none;">'.$nl);
if(fxIsArray($pa))
{
	foreach($pa as $filename => $d)
	{
		$sadd='';
		if($d['hr'])
			$sadd='border-top:1px solid #dddddd;margin-top:8px;padding-top:4px;';
		echo('			<div class="favlink" style="position:relative;cursor:pointer;'.$sadd.'" '.fxf_jsFunction('loadManual', 0, $filename).'>');
		echo('<img src="'.$GLOBALS['gfxpath'].'dt_pdf.png" style="position:absolute;left:10px;top:3px;"><b>&uarr;</b>&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;'.$d['name']);
		if($d['size'] > 0)
			echo('&nbsp;<i class="s2 grey">('.$d['size'].'&nbsp;MB)</i>');
		echo('</div>'.$nl);
	}
}
echo('			<br>'.$nl);
echo('		</div>'.$nl);

// ...Menu: Info
echo('		<div id="hb_min" style="position:absolute;left:2px;top:2px;width:'.$dim_menu.'px;bottom:2px;line-height:22px;border:1px solid #dddddd;border-radius:4px;background:#ffffff;padding:4px;white-space:nowrap;overflow:auto;display:none;">'.$nl);
if(fxIsArray($ia))
{
	foreach($ia as $filename => $d)
	{
		$sadd='';
		if($d['hr'])
			$sadd='border-top:1px solid #dddddd;margin-top:8px;padding-top:4px;';
		if(strlen($d['tr']))
		{
			$oc=fxf_jsFunction('loadManual', $d['tr'], '');
			$rarr='';
		}
		else
		{
			$oc=fxf_jsFunction('loadManual', 0, $filename);
			$rarr='&nbsp;<div style="font-size:16px;margin-right:12px;display:inline;">&rarr;</div>';
		}
		echo('			<div class="favlink" style="margin-top:4px;cursor:pointer;'.$sadd.'" '.$oc.'>'.$nl);
		echo('				'.$rarr.$d['name']);
		if($d['size'] > 0)
			echo('&nbsp;<i class="s2 grey">('.$d['size'].'&nbsp;MB)</i>');
		echo('			</div>'.$nl);
	}
}
echo('			<br>'.$nl);
echo('		</div>'.$nl);

// ...Text
$trf=0;
if(fxIsArray($GLOBALS['fxpglobals']['trstack']))
{
	foreach($GLOBALS['fxpglobals']['trstack'] as $ltr)
	{
		if($ltr != $GLOBALS['fxpglobals']['tr'])
		{
			$trf=$ltr;
			break;
		}
	}
}
//fxDebug($trf,'Jump back to: $trf');
echo('		<div id="hb_txt" style="position:absolute;left:'.($dim_menu+18).'px;top:2px;right:2px;bottom:2px;border:1px solid #dddddd;border-radius:4px;background:#ffffff;padding:4px;white-space:nowrap;overflow:auto;">'.$nl);
$_GET['mtr']=$trf;
include('popup_hlp.inc');
echo('		</div>'.$nl);

echo('	</div>'.$nl);

echo('</div>'.$nl);
?>