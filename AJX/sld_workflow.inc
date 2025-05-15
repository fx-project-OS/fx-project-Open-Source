<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : sld_workflow.inc                                             //
// Version     : 24.1                                                         //
// Begin       : 2020-10-01                                                   //
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
 * Slider: "WORKFLOW"
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
 */

// Program can only be called from javascript/ajax
$callOK=0;
if(is_array($_GET) && isset($_GET['mod']) && isset($_GET['lng']))
	$callOK=1;
if(!$callOK)
	die();

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

// Set variables
$ret='';
$mod=(int)$_GET['mod'];
$wid=$_GET['wid'];
$lng=(int)$_GET['lng'];

// Dynamic variables
// ...Workflow color mode: 0=B/W, 1=Color
if(fxIsArray($_GET) && isset($_GET['col']))
	$col=max(0,min(1,(int)$_GET['col']));
else
	$col=(int)$GLOBALS['fxpglobals']['setwfcolor'];
//$col=0;
//$col=1;
// ...Workflow action icon row mode: 0=in 1 row (all 4 icons), 1=in 2 rows (2 icons per row)
if(fxIsArray($_GET) && isset($_GET['aim']))
	$aim=max(0,min(1,(int)$_GET['aim']));
else
	$aim=(int)$GLOBALS['fxpglobals']['setwfairow'];
$rdm=4;
if($aim)
	$rdm=2;

// Get menu
$vt=array();
// Load authorized program functions
$wa=loadMenu(false,true,false);
if(fxIsArray($wa))
{
	foreach($wa as $tk => $tc)
	{
		if($tc['tr'])
			$vt[$tc['tr']]=true;
	}
}
//fxDebug($vt,'$vt');

// Load all possible program functions?
$disp_all=strtolower(substr((string)$GLOBALS['fxpglobals']['settings']['alle_tr_anzeig'],0,1));
//$disp_all='n';
if($disp_all !== 'n')
	$wa=loadMenu(true,false,false);
if(!fxIsArray($wa))
	die('');
//fxDebug($wa,'$wa',0);

$ms=0;
foreach($wa as $id => $ma)
{
	$wa[$id]['op']=0.5;
	if($disp_all !== 'n')
		$wa[$id]['disp']=true;
	else
		$wa[$id]['disp']=false;
	if(!$ma['lv'] && ($ma['tx'] != 'Sysadmin'))
		$ms++;
}

// Get program function rights
$ra=getAllProgramRights();
//fxDebug($ra,'$ra', 0);

// Only display authorized program functions?
foreach($wa as $id => $ma)
{
	if($ma['tr'] && isset($ra[$ma['tr']]) && is_array($ra[$ma['tr']]) && isset($ra[$ma['tr']]['tr']) && $ra[$ma['tr']]['tr'])
	{
		$did=$id;
		while(strlen((string)$did) > 2)
		{
			$wa[$did]['disp']=true;
			if(!strlen((string)$ma['eb']))
				$wa[$did]['op']=1.0;
			$did=substr((string)$did,0,-3);
		}
	}
}

// Get actual css design, extract styles from css and interpolate menu colors
$css=fxpCSSDesign();
//echo('<pre>'.$css.'</pre>');
$dsa=fxpClientDesign($css,$ms);
//fxDebug($dsa,'$dsa: $css='.$css.', $ms='.$ms);

$m0col=$dsa['cmenu1-i']['background'];
$m1col=array();
$c=1;
if(fxIsArray($dsa))
{
	while(fxIsArray($dsa['cmenu-c'.$c]))
	{
		if(isset($dsa['cmenu-c'.$c]['background']) && strlen((string)$dsa['cmenu-c'.$c]['background']) && ($dsa['cmenu-c'.$c]['background'] != 'transparent'))
			$m1col[]=$dsa['cmenu-c'.$c]['background'];
		$c++;
	}
}
//fxDebug($m1col,'$m1col');
$mca=getMenuColors($ms, $m0col, $m1col);
//fxDebug($mca,'$mca');

// Create return string
switch($mod)
{
	case 0:	// Main menu
		$hd='';
		$ws='';

		$sdisp=false;
		if(!$GLOBALS['fxpglobals']['person'])
			$sdisp=true;
		foreach($wa as $id => $ma)
		{
			if($ma['disp'] && !$ma['lv'] && ($sdisp || ($ma['tx'] != 'Sysadmin')))
			{
				if(!strlen((string)$hd) && strlen((string)$ma['wq']))
					$hd=$ma['wq'];

				$mt=$ma['tx'];
				$mi=$ma['im'];
				if(($col && !file_exists($GLOBALS['gfxpath'].$mi.'_c_80x80.png')) || (!$col && !file_exists($GLOBALS['gfxpath'].$mi.'_b_80x80.png')))
					$mi='wtf';
				$m2=$mca[1][(int)substr((string)$id,1,2)][0]['background'];
				$m1=$mca[1][(int)substr((string)$id,1,2)][3]['background'];
				$brd='';
				if($ma['op'] < 1.0)
					$brd='border-color:#e43d50;border-right-color:#e1001a;border-bottom-color:#e1001a;opacity:'.$ma['op'].';';

				if(strlen((string)$ws))
					$ws .= '°';
				$ws .= $id.'|'.$mt.'|'.$mi.'|'.$m1.'|'.$m2.'|'.$brd;
			}
		}
		$ret=$hd.'^'.$ws;
	break;

	case 1: // Submenu
		$hd='';
		$ws='';

		$wl=strlen((string)$wid);
		$la=substr_count($wid,'_');
		$ln=$la+1;

		$all='';
		$dummy='';
		foreach($wa as $id => $ma)
		{
			if($ma['disp'] && ($ma['lv'] == $ln) && (substr((string)$id,0,$wl) == $wid) && $ma['tr'])
			{
				$dummy=$id;
				break;
			}
		}
//$hd .= '<br />&middot; $dummy='.$dummy;

		foreach($wa as $id => $ma)
		{
//$hd .= '<br />&middot; $id='.$id.', lv='.$ma['lv'].', tr='.$ma['tr'].' -- tx='.$ma['tx'];
			if($ma['disp'] && ($ma['lv'] == $ln) && (substr((string)$id,0,$wl) == $wid))
			{
//$hd .= '<br />&middot; $id='.$id.', lv='.$ma['lv'].', tr='.$ma['tr'].' -- tx='.$ma['tx'];
				if(!strlen((string)$hd) && strlen((string)$ma['wq']))
					$hd=$ma['wq'];

				$m1='#f0f0f0';
				$m2=$mca[1][(int)substr((string)$id,1,2)][0]['background'];
				$m1=$mca[1][(int)substr((string)$id,1,2)][3]['background'];
				$brd='';
				if($ma['op'] < 1.0)
					$brd='border-color:#e43d50;border-right-color:#e1001a;border-bottom-color:#e1001a;opacity:'.$ma['op'].';';

				if(!strlen((string)$all))
				{
					$all=substr((string)$id,0,3);
					if(strlen((string)$ws))
						$ws .= '°';
					$ws .= $all.'|&darr;&nbsp;<i class=green>'.meldung(1422).'</i>&nbsp;&darr;|all|'.$m1.'|'.$m2.'|'.$brd;
				}
				if(strlen((string)$dummy))
				{
					if(strlen((string)$ws))
						$ws .= '°';
					$ws .= $dummy.'|&darr;&nbsp;<i class=blue>Standard</i>&nbsp;&darr;|dvd|'.$m1.'|'.$m2.'|'.$brd;
					$dummy='';
				}

				if(!$ma['tr'])
				{
					$mt=$ma['tx'];
					$mi=$ma['im'];

					if(($col && !file_exists($GLOBALS['gfxpath'].$mi.'_c_80x80.png')) || (!$col && !file_exists($GLOBALS['gfxpath'].$mi.'_b_80x80.png')))
						$mi='wtf';
					if(strlen((string)$ws))
						$ws .= '°';
					$ws .= $id.'|'.$mt.'|'.$mi.'|'.$m1.'|'.$m2.'|'.$brd;
				}
			}
		}
//$hd .= '<hr />$mod='.$mod.', $wid='.$wid.', $lng='.$lng.' -- $wl='.$wl.', $la='.$la.', $ln='.$ln;
		$ret=$hd.'^'.$ws;
	break;

	case 2: // Content
		$ct='<table width="99%" border="0" cellpadding="8" cellspacing="0">'.$GLOBALS['nl'];	// Continuative Transactions (Weiterführende Transaktionen)
		$cw=' width="100%"';
		$hd='';
		$ws='';
		$tp='';
		$wl=strlen((string)$wid);
		$la=substr_count($wid,'_');
		$ln=$la+1;
		$ic='b';
		if($col)
			$ic='c';
		$tra=array('"'=>'&quot;', "'"=>'´');
		$m2=$mca[1][(int)substr((string)$wid,1,2)][0]['background'];
		$m1=$mca[1][(int)substr((string)$wid,1,2)][3]['background'];

		$dummy='';
		if($wa[$wid]['tr'])
			$dummy=substr((string)$wid,0,3);
		$ds=strlen((string)$dummy);

		$all='';
		if(strlen((string)$wid) == 3)
			$all=$wid;
		$as=strlen((string)$all);

//$hd .= '<br />&middot; $wid='.$wid.', $dummy='.$dummy.', $ds='.$ds;

		// Actions
		$aa=array(FXP_DISPLAY=>'sh', FXP_CREATE=>'ne', FXP_CHANGE=>'ch', FXP_DELETE=>'de');
		$ad=array(FXP_CREATE, FXP_CHANGE, FXP_DISPLAY, FXP_DELETE);
		// ...get literals in current language
		$stla=fxGetLanguageTexts('start');
//fxDebug($stla,'$stla: fxGetLanguageTexts(\'start\')',0);
		$acla=array();
		foreach($aa as $am => $dummy)
		{
			$acla[$am]=$stla['actreg'.$am.'^ll'];		// Long literal text for each action mode, e.g. Display, Create, ...
			$acla[$am+10]=$stla['actreg'.$am.'^ls'];	// Literal abbreviation for each action mode, e.g. Disp., New, ...
		}
		$hrc=0;
		foreach($wa as $id => $ma)
		{
//$hd .= '<br />&middot; $id='.$id.', lv='.$ma['lv'].', tr='.$ma['tr'].' -- tx='.$ma['tx'];
			if($ma['disp'] && $ma['tr'] && (($as && (substr((string)$id,0,$as) == $all)) || ($ds && ($ma['lv'] < $ln) && (substr((string)$id,0,$ds) == $dummy)) || (!$ds && ($ma['lv'] == $ln) && (substr((string)$id,0,$wl) == $wid))))
			{
//fxDebug($ma,$id);
				// Horizontale Trennlinie?
				if($hrc)
					$ct .= '	<tr><td colspan="2"><hr style="border:0;height:1px;border-top:1px solid #aaaaaa;" /></td></tr>';

				$lt=$ma['tr'];
				$tx=strtr($ma['tx'],$tra);
				$tt=$tx;
				if(strlen((string)$ma['tt']))
					$tt=strtr($ma['tt'],$tra);
				$dw=$tt;
				if(strlen((string)$ma['dw']))
					$dw=strtr($ma['dw'],$tra);
				$mk=$ma['mk'];
				$ac=$ma['ac'];
				$di=$ma['disp'];
				$brd='';
				if($ma['op'] < 1.0)
					$brd='border-color:#e43d50;border-right-color:#e1001a;border-bottom-color:#e1001a;opacity:'.$ma['op'].';';
				$da=0;
				foreach($ad as $am)
				{
					$a2=pow(2,$am);
					if(($ac & $a2) && isset($ra[$lt]) && ($ra[$lt]['tr'] & $a2))
					{
						$da=$am;
						break;
					}
				}
//$hd .= '<b>'.$lt.'</b>: '.$tx.'<br /><br />&quot;'.$dw.'&quot;<br /><br />$id='.$id.', lv='.$ma['lv'].', ac='.$ac.', da='.$da.', di='.$di.'<hr />';
//if(!strlen((string)$hd) && strlen((string)$ma['wq'])) $hd=$ma['wq'];

				// Left column: Action icons
				$apl=0;
				$apt=0;
				$awh=22+$rdm*4;
				$aws=10;
				$c0='<div id="fxm_'.$id.'_ap" style="position:relative;width:'.($rdm*($awh+$aws)-$aws).'px;margin-right:16px;margin-top:4px;">';
				foreach($aa as $am => $ai)
				{
					$a2=pow(2,$am);
					$dcls=' class="ainactive"';
					$ccls='act';
					$tlnd='';
					$tlni='';
					$dtt='';
					$style='width:'.$awh.'px;height:'.$awh.'px;margin-right:4px;';
					if(($ac & $a2) && isset($ra[$lt]) && ($ra[$lt]['tr'] & $a2))
					{
						$tb=''; //$tb=' -- ['.$am.'-'.$a2.'/'.$ac.' -- '.$da.']';
						$dcls='';
						$ccls='sel';
						$tlnd=' id="fxm_'.$id.'_td"';
						$tlni=' tr="'.$lt.'" arg="aktion='.$am.'"';
						$dtt=' tooltip="<b>'.$acla[$am].'</b>: '.$tx.' <i class=grey>('.$tp.')</i>'.$tb.'"';
						$style .= 'cursor:pointer;';
					}
					$c0 .= '<div'.$tlnd.$tlni.$dcls.$dtt.' style="position:absolute;top:0;left:'.$apl.'px;top:'.$apt.'px;'.$style.'">';
					$c0 .= '<div class="wfmcircle1_'.$ccls.'" style="background:linear-gradient(to bottom right,'.$m1.','.$m2.');border-width:3px;'.$brd.'"><div class="wfmcircle2" style="border:1px solid '.$m2.';"><img id="fxm_wfmcmii'.$lt.'_'.$am.'"'.$tlni.' class="wfmimage" src="'.$GLOBALS['gfxpath'].'ac'.$am.'_'.$ic.'_80x80.png" style="left:19%;top:19%;"></div></div>';
					$c0 .= '</div>';

					$apl += $awh+$aws;
					if(($am == FXP_CREATE) && ($rdm == 2))
					{
						$apl=0;
						$apt += $awh+$aws;
					}
				}
				// Right column: Name and description
				$rcol='222233';
				$ropa='';
				if($ma['op'] < 1.0)
				{
					$rcol='e1001a';
					$ropa='opacity:'.$ma['op'].';';
				}
				$c1  = '<div style="psoition:relative;min-height:'.(($awh+$aws)*(4-$rdm)).'px;">';
				$c1 .= '	<div id="fxm_'.$id.'_tr" tr="'.$lt.'" arg="aktion='.$da.'" style="float:left;color:#'.$rcol.';font-size:175%;font-weight:bold;'.$ropa.'">'.$tx.' <i>('.$tp.')</i></div>';
				$c1 .= '	<br /><br /><br /><p align="justify" style="padding:0;margin:0;margin-right:10px;color:#555566;font-size:11pt;'.$ropa.'">'.$dw.'</p>';
				$c1 .= '</div>';

				// ...Mask screenshot tooltip
				$mwidth=400;
				$ttt=' tooltip="<hide><div style=\'width:'.$mwidth.'px;margin-left:40px;margin-bottom:2px;border:1px solid #444455;border-left:1px solid #aaaaaa;border-top:1px solid #aaaaaa;border-radius:6px;background:#006b9f;color:#ffffff;white-space:normal;padding:4px;opacity:0.9;box-shadow:4px 4px 6px rgba(0,0,0, 0.50);\'><b>'.$tt.'</b></div>';
				$ts='';
				$msa=array(
					$GLOBALS['docpath'].substr('00'.$lng,-2).'/SCR/'.substr('000000'.$lt,-6).'_'.substr('000000'.$mk,-6).'.png',
					$GLOBALS['docpath'].substr('00'.$lng,-2).'/SCR/'.substr('000000'.$lt,-6).'_000000.png'
				);
				foreach($msa as $msp)
				{
					if(file_exists($msp))
					{
						$ts=$msp;
						break;
					}
				}
				if(strlen((string)$ts))
					$ttt .= '<div style=\'width:'.$mwidth.'px;margin-left:40px;margin-bottom:40px;border:1px solid #444455;border-left:1px solid #aaaaaa;border-top:1px solid #aaaaaa;border-radius:6px;background:#ffffff;padding:4px;opacity:0.9;box-shadow:4px 4px 6px rgba(0,0,0, 0.50);\'><img src=\''.$ts.'\' width='.$mwidth.'></div>';
				$ttt .= '"';

				$joa=' onmouseover="$(this).style.background=\'rgba(255,255,255, 0.25)\';$(\'fxm_'.$id.'_tx\').style.color=\'#006b9e\';$(\'fxm_'.$id.'_tx\').style.textShadow=\'-1px -1px #ffffff\';" onmouseout="$(this).style.background=\'\';$(\'fxm_'.$id.'_tx\').style.color=\'#222233\';$(\'fxm_'.$id.'_tx\').style.textShadow=\'\';"';

				$ct .= '<tr id="fxm_'.$id.'_tr" tr="'.$lt.'"  arg="aktion='.$da.'"'.$joa.' style="cursor:pointer;"><td valign="top" nowrap>'.$c0.'</td><td valign="top"'.$cw.$ttt.'>'.$c1.'</td></tr>';

				$hrc++;
			}
			else
				$tp=strtr($ma['tx'],$tra);
		}
		$ct .= '</table>';

		$ret=$hd.$ct;
	break;
}

echo($ret);
?>