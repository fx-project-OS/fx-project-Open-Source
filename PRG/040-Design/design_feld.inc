<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : design_feld.inc                                              //
// Version     : 21.1                                                         //
// Begin       : 2021-02-01                                                   //
// Last Change : 2021-04-15                                                   //
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

$c=0;
$p=0;
$i='';
$i4='';
$v='';
$v3='';
$n='';
$d='';
$f='';
$m=0;
$s=0;
if(fxIsArray($_GET))
{
	// Client
	if(isset($_GET['c']))
		$c=(int)$_GET['c'];
	// Person
	if(isset($_GET['p']))
		$p=(int)$_GET['p'];
	// Field
	if(isset($_GET['i']))
	{
		$i=$_GET['i'];
		$i4=substr($i,0,4);
	}
	// Value
	if(isset($_GET['v']))
	{
		$v=strtolower($_GET['v']);
		$v3=substr($v,0,3);
	}
	// Name
	if(isset($_GET['n']))
		$n=$_GET['n'];
	// Design
	if(isset($_GET['d']))
		$d=$_GET['d'];
	// Filename
	if(isset($_GET['f']))
		$f=$_GET['f'];
	// Max. menu sections
	if(isset($_GET['m']))
		$m=(int)$_GET['m'];
	// Section
	if(isset($_GET['s']))
		$s=(int)$_GET['s'];
}

if(!$c || !isset($_GET['p']) || !strlen($i) || !strlen($v) || !strlen($d) || !strlen($f) || !$m || !$s)
	die;

$req_fn=fxf_fn_reqFilename('design_functions', 40);
if(strlen($req_fn))
	require($req_fn);

$rs_css='';
$rs_field='';
$rs_menu='';
ob_start();

echo('<u>Parameters:</u><br />&middot; Client: $c=<b>'.$c.'</b><br />&middot; Person: $p=<b>'.$p.'</b><br />&middot; Field: $i=<b>'.$i.'</b> ($i4=<b>'.$i4.'</b>)<br />&middot; Value: $v=<b>'.$v.'</b> ($v3=<b>'.$v3.'</b>)<br />&middot; Name: $n=<b>'.$n.'</b><br />&middot; Design: $d=<b>'.$d.'</b><br />&middot; Filename: $f=<b>'.$f.'</b><br />&middot; Max. menu sections: $m=<b>'.$m.'</b><br />&middot; Section: $s=<b>'.$s.'</b><hr size=1 /><br />'.$nl);

// Get temp user design
$tcss=fxLoad($f);
echo('<u>Temp user design <b>$tcss</b> = fxLoad(\''.$f.'\'):</u><br /><pre>'.fxHtmlEncode($tcss).'</pre><hr size=1 /><br />'.$nl);
$dsa=fxpClientDesign($tcss,$m);
fxDebug($dsa,'$dsa = fxpClientDesign($tcss,$m);', 0);

// Add, change or delete menu color value
if($i4 == 'mset')
{
	$msa=substr('0'.$s,-2);
	$act=FXP_CHANGE;
	$dkey=substr($i,5);
	$ovalue=$dsa[$dkey]['background'];
	if($dkey == 'cmenu1-i')
	{
		$cidx=-1;
		$nvalue=$v;
	}
	else
	{
		$cidx=(int)substr($i,12);
		if(!strlen($ovalue))
		{
			$act=FXP_CREATE;
			$nvalue=$dsa['cmenu-c'.($cidx-1)]['background'];
		}
		else if(($v == 'del') || ($v == 'transparent'))
		{
			$act=FXP_DELETE;
			$nvalue='';
			$c=$cidx;
			while($c < $m)
			{
				$dsa['cmenu-c'.$c]['background']=$dsa['cmenu-c'.($c+1)]['background'];
				$c++;
			}
			$dsa['cmenu-c'.$c]['background']='';
		}
		else
			$nvalue=$v;
	}
echo('<u>Menu color value:</u><br />&middot; $act=<b>'.$act.'</b><br />&middot;$cidx=<b>'.$cidx.'</b><br />&middot; $dkey=<b>'.$dkey.'</b><hr size=1 /><br />'.$nl);
echo('$ovalue=<b>'.$ovalue.'</b><br />&rarr; $nvalue=<b>'.$nvalue.'</b><hr size=1 /><br />'.$nl);

	// Change stylesheet setting
	if(strlen($nvalue))
		$dsa[$dkey]['background']=$nvalue;
fxDebug($dsa,'$dsa = fxpClientDesign($tcss,$m);', 0);

	// Create changed stylesheet and save it
	$ntcss=fxpCreateClientCSS($n,$dsa,$m);
echo('<u>Change dtemp user design <b>$ntcss</b>:</u><br /><pre>'.fxHtmlEncode($ntcss).'</pre><hr size=1 /><br />'.$nl);
	fxSave($f,$ntcss,true);
	$rs_css=$ntcss;
	if(strlen($nvalue))
		$dsa[$dkey]['background']=$nvalue;

	// Menu Preview
	$mp=getDesignPreview($dsa,$msa,true);

	// Menu Design Editor: Description, Setting + Preview
	$mda=getMenuDesignEditor($dsa,$msa,$ms,$lang);
	$rs_menu=$mp.'!-m-!'.$mda['dsc'].'!-m-!'.$mda['col'].'!-m-!'.$mda['prv'];
}

// Menu section preview
else if($i4 == 'msec')
{
	$msa=substr('0'.$s,-2);

	// Menu preview
	$mp=getDesignPreview($dsa,$msa,true);
	$rs_menu=$mp;
}

// Return field info
else if($i4 == 'Feld')
{
	if(substr($v,0,3) == 'dp_')
		$dkey=substr($v,3);
	else
		$dkey=$v;

	// Description + Setting
	$a=getDesignEditor($dsa, $dkey);
	$rs_field=$a['dsc'].'|'.$a['set'];
}

// Change direction of the color gradient
else if($v3 == 'dir')
{
	$ndir=(int)substr($v,3);
	$inam=substr($i,4);
	$ia=explode('_', $inam);
	$dkey=trim($ia[0]);
	$skey=trim($ia[1]);
echo('<u>Change direction of the color gradient:</u><br />&middot; $ndir=<b>'.$ndir.'</b><br />&middot; $dkey=<b>'.$dkey.'</b><br />&middot; $skey=<b>'.$skey.'</b><hr size=1 /><br />'.$nl);

	$nvalue='';
	if($ndir != 2)
		$nvalue=$ndir.'|';
	$ovalue=$dsa[$dkey][$skey];
	if(substr($ovalue,1,1) == '|')
		$nvalue .= substr($ovalue,2);
	else
		$nvalue .= $ovalue;
echo('$ovalue=<b>'.$ovalue.'</b><br />&larr; $nvalue=<b>'.$nvalue.'</b><hr size=1 /><br />'.$nl);

	// Change stylesheet setting
	$dsa[$dkey][$skey]=$nvalue;
fxDebug($dsa,'$dsa = fxpClientDesign($tcss,$m);', 0);

	// Create changed stylesheet and save it
	$ntcss=fxpCreateClientCSS($n,$dsa,$m);
echo('<u>Change dtemp user design <b>$ntcss</b>:</u><br /><pre>'.fxHtmlEncode($ntcss).'</pre><hr size=1 /><br />'.$nl);
	fxSave($f,$ntcss,true);
	$rs_css=$ntcss;

	// Description + Setting
	$a=getDesignEditor($dsa, $dkey);
	$rs_field=$a['dsc'].'|'.$a['set'];
}

// Add new color value
else if($v3 == 'add')
{
	$inam=substr($i,4);
	$cidx=0;
	$ibp=strpos($inam,'[');
	if($ibp)
	{
		$cidx=(int)substr($inam,$ibp+1,-1);
		$inam=substr($inam,0,$ibp);
	}
	$ncol=substr($v,3);
	$ia=explode('_', $inam);
	$dkey=trim($ia[0]);
	$skey=trim($ia[1]);
echo('<u>Add new color value:</u><br />&middot; $cidx=<b>'.$cidx.'</b><br />&middot; $ncol=<b>'.$ncol.'</b><br />&middot; $dkey=<b>'.$dkey.'</b><br />&middot; $skey=<b>'.$skey.'</b><hr size=1 /><br />'.$nl);

	$ovalue=$dsa[$dkey][$skey];
	$nvalue=$ovalue.','.$ncol;
echo('$ovalue=<b>'.$ovalue.'</b><br />&larr; $nvalue=<b>'.$nvalue.'</b><hr size=1 /><br />'.$nl);

	// Change stylesheet setting
	$dsa[$dkey][$skey]=$nvalue;
fxDebug($dsa,'$dsa = fxpClientDesign($tcss,$m);', 0);

	// Create changed stylesheet and save it
	$ntcss=fxpCreateClientCSS($n,$dsa,$m);
echo('<u>Change dtemp user design <b>$ntcss</b>:</u><br /><pre>'.fxHtmlEncode($ntcss).'</pre><hr size=1 /><br />'.$nl);
	fxSave($f,$ntcss,true);
	$rs_css=$ntcss;

	// Description + Setting
	$a=getDesignEditor($dsa, $dkey);
	$rs_field=$a['dsc'].'|'.$a['set'];
}

// Delete color value
else if($v3 == 'del')
{
	$inam=substr($i,4);
	$cidx=0;
	$ibp=strpos($inam,'[');
	if($ibp)
	{
		$cidx=(int)substr($inam,$ibp+1,-1);
		$inam=substr($inam,0,$ibp);
	}
	$ia=explode('_', $inam);
	$dkey=trim($ia[0]);
	$skey=trim($ia[1]);
echo('<u>Delete color value:</u><br />&middot; $cidx=<b>'.$cidx.'</b><br />&middot; $dkey=<b>'.$dkey.'</b><br />&middot; $skey=<b>'.$skey.'</b><hr size=1 /><br />'.$nl);

	$ovalue=$dsa[$dkey][$skey];
	$oa=explode(',', $ovalue);
	$cc=0;
	$ndir='';
	$nvalue='';
	foreach($oa as $oc => $ov)
	{
		if(substr($ov,1,1) == '|')
		{
			$ndir=substr($ov,0,2);
			$ov=substr($ov,2);
		}
		if($oc != $cidx)
		{
			$cc++;
			if(strlen($nvalue))
				$nvalue .= ',';
			$nvalue .= $ov;
		}
	}
	if($cc > 1)
		$nvalue=$ndir.$nvalue;
echo('$ovalue=<b>'.$ovalue.'</b><br />&larr; $nvalue=<b>'.$nvalue.'</b><hr size=1 /><br />'.$nl);

	// Change stylesheet setting
	$dsa[$dkey][$skey]=$nvalue;
fxDebug($dsa,'$dsa = fxpClientDesign($tcss,$m);', 0);

	// Create changed stylesheet and save it
	$ntcss=fxpCreateClientCSS($n,$dsa,$m);
echo('<u>Change dtemp user design <b>$ntcss</b>:</u><br /><pre>'.fxHtmlEncode($ntcss).'</pre><hr size=1 /><br />'.$nl);
	fxSave($f,$ntcss,true);
	$rs_css=$ntcss;

	// Description + Setting
	$a=getDesignEditor($dsa, $dkey);
	$rs_field=$a['dsc'].'|'.$a['set'];
}

// Change color value
else if($i4 == 'set_')
{
	$inam=substr($i,4);
	$cidx=0;
	$ibp=strpos($inam,'[');
	if($ibp)
	{
		$cidx=(int)substr($inam,$ibp+1,-1);
		$inam=substr($inam,0,$ibp);
	}
	$ia=explode('_', $inam);
	$dkey=trim($ia[0]);
	$skey=trim($ia[1]);
echo('<u>Change color value:</u><br />&middot; $cidx=<b>'.$cidx.'</b><br />&middot; $dkey=<b>'.$dkey.'</b><br />&middot; $skey=<b>'.$skey.'</b><hr size=1 /><br />'.$nl);

	$ovalue=$dsa[$dkey][$skey];
	$oa=explode(',', $ovalue);
	$ndir='';
	$nvalue='';
	foreach($oa as $oc => $ov)
	{
		if(substr($ov,1,1) == '|')
		{
			$ndir=substr($ov,0,2);
			$ov=substr($ov,2);
		}
		if(strlen($nvalue))
			$nvalue .= ',';
		if($oc == $cidx)
			$nvalue .= $v;
		else
			$nvalue .= $ov;
	}
	$nvalue=$ndir.$nvalue;
echo('$ovalue=<b>'.$ovalue.'</b><br />&larr; $nvalue=<b>'.$nvalue.'</b><hr size=1 /><br />'.$nl);

	// Change stylesheet setting
	$dsa[$dkey][$skey]=$nvalue;
fxDebug($dsa,'$dsa = fxpClientDesign($tcss,$m);', 0);

	// Create changed stylesheet and save it
	$ntcss=fxpCreateClientCSS($n,$dsa,$m);
echo('<u>Change dtemp user design <b>$ntcss</b>:</u><br /><pre>'.fxHtmlEncode($ntcss).'</pre><hr size=1 /><br />'.$nl);
	fxSave($f,$ntcss,true);
	$rs_css=$ntcss;

	// Description + Setting
	$a=getDesignEditor($dsa, $dkey);
	$rs_field=$a['dsc'].'|'.$a['set'];
}

// Display debug?
$ds=ob_get_contents();
ob_end_clean();
echo $ds;

// Return result
echo $GLOBALS['_divstr'][0].$rs_css.$GLOBALS['_divstr'][1].$rs_field.$GLOBALS['_divstr'][2].$rs_menu;
?>