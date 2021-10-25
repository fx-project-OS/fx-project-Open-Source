<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : popup_col.inc                                                //
// Version     : 21.2                                                         //
// Begin       : 2020-10-29                                                   //
// Last Change : 2020-10-29                                                   //
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
 * Mask: "Set columns to be displayed"
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

$callOK=0;
if(fxIsArray($_GET) && isset($_GET['mn']))
{
	if(isset($_GET['ww']) && isset($_GET['wh']) && isset($_GET['fs']))
		$callOK=1;
	else if(fxIsArray($_POST) && isset($_POST['action']) && isset($_POST['rcsel']) && isset($_POST['rctxt']))
		$callOK=2;
}
if(!$callOK)
	die();

if($GLOBALS['fxpglobals']['tr'] != 0)
{
	setForm(false);	// Always select mask

	// Call variables
	$um=trim($_GET['mn']);
	$ww=800;
	$wh=600;
	$fs='';
	$action='';
	if($callOK == 1)
	{
		$ww=(int)$_GET['ww']-64;
		$wh=(int)$_GET['wh'];
		$fs=trim($_GET['fs']);
	}
	else
	{
		$action=strtolower(trim($_POST['action']));
		$rcse=explode('_',$_POST['rcsel']);
		$fs=trim($rcse[1]);
	}
	$fsl=strlen($fs);

	if(substr($um,0,1) == 'u')	// Maskengenerator
		$mn=-1*(int)substr($um,1);
	else
	{
		$mn=(int)$um;
 		if(substr($um,0,1) == '-')
	 		$um='u'.substr($um,1);
	}
//echo('<div style="background:#ffff00;">$um='.$um.', $mn='.$mn.', $ww='.$ww.', $wh='.$wh.'<br>$fs='.$fs.', $fsl='.$fsl.'<br>$action='.$action.'</div>');

	$fw=256;			// Column width
	$cw=20;				// Checkbox column width
	$nw=40;				// Number column width
	$tw=$fw-$cw-$nw;	// Text column width

	$cl=floor(($ww-70)/$fw);	// Columns
	$ll=ceil($fsl/$cl);			// Lines

	$rbw=470;	// Width Icons + Buttons
	$isw=max(200, $cl*$fw-$rbw);
	$puw=$isw+$rbw+70;
//echo('<div style="background:#00ffff;">$fw='.$fw.', $cw='.$cw.', $nw='.$nw.', $tw='.$tw.'<br>$cl='.$cl.', $ll='.$ll.', $rbw='.$rbw.', $isw='.$isw.', $puw='.$puw.'</div>');

	$def='';
	$all='';
	$ids=array();
	$idsr=array();
	$farr=array();
	if($mn)
	{
		// Get all report columns
//fxDebug($GLOBALS['fxpglobals']['tr'], 'tr');
		if($GLOBALS['fxpglobals']['tr'] == 189)	// Matrix
			dummyMask($mn);
		else if(!fxIsArray($GLOBALS['fx_mfields'][$mn]))
		{
			sessionMask($mn);
		}
//fxDebug($GLOBALS['fx_mfields'][$mn], 'fx_mfields');
		if(fxIsArray($GLOBALS['fx_mfields'][$mn]))
		{
			if($GLOBALS['fxpglobals']['tr'] == 152)	// Transactions with possible additional user fields: 152=Internal Project Report
			{
				prj_userfields();
				if($GLOBALS['unumber'])
					add_usermask_to_mask($GLOBALS['unumber'], 349);
			}

			$fp=0;
			$fn=1;
			$fl=0;
			$fc=0;
			foreach($GLOBALS['fx_mfields'][$mn] as $f)
			{
				$valid=false;
				if((substr($um,0,1) == 'u') || ($GLOBALS['fxpglobals']['tr'] < 0))	// Mask generator
				{
					if(($f['anzeigeart'] == 6) || ($f['anzeigeart'] == 7) || ($f['anzeigeart'] == 8) || ($f['anzeigeart'] == 10) || ($f['anzeigeart'] == 11))
						$valid=true;
				}
				else if($f['zeile'] == 1)
					$valid = true;

				if($valid)
				{
//fxDebug($f, $fp);
					$id=(int)$f['eingabefeld'];
					if($idsr[$id])
						continue;
					$idsr[$id]=true;

					// Default
					if((substr($um,0,1) == 'u') || ($GLOBALS['fxpglobals']['tr'] < 0))	// Mask generator
					{
						if(substr($f['feldname'],-5) == '_Name')
							$val=3;
						else
							$val=1;
					}
					else
						$val=(int)$f['defaultwert'];
					$def .= $val;

					// IDs
					$ids[$fp]=array('id'=>$id, 'def'=>$val);

					// All
					if(!$val)
						$val=1;
					$all .= $val;

					$hfs=(int)substr($fs,$fp,1);
//echo('<div style="background:#ffffff;">'.$fn.': <b>'.$f['feldname'].'</b> -- $val='.$val.', $hfs='.$hfs.'<br />');
					if($hfs == 4)
					{
						$fp++;
						continue;
					}

					$disabled='';
					if($hfs > 1)
						$disabled='yes';

					$checked='';
					if($hfs%2 == 1)
						$checked='1';

					$col='grey';
					if(!strlen($disabled))
						$col='black';

					if(!fxIsArray($farr[$fc]))
						$farr[$fc]=array();

					// Spaltennummer
					$farr[$fc][$fl]  = '<td width="'.$nw.'" align="right" valign="top" nowrap><span class="lightgrey">'.$fn.':</span></td>';
					// Checkbox
					$farr[$fc][$fl] .= '<td width="'.$cw.'" valign="top">'.fieldCheckbox('rccb_'.$um.'_'.$f['feldname'], '', $checked, false, $disabled).'</td>';
					// Spaltenname
					$l=getLiteral($f);
					if(strlen($l['lit']))
						$lit=$l['lit'];
					else
						$lit=$f['literal_value'];
					$farr[$fc][$fl] .= '<td width="'.$tw.'" valign="top" nowrap><font class="'.$col.'">'.$lit.'</font></td>';

					$fp++;
					$fn++;
					$fl++;
					if($fl >= $ll)
					{
						$fl=0;
						$fc++;
					}
				}
			}
		}
	}

	// Popup mask
	if($callOK == 1)
	{
		$mtitle=meldung(565,true);	// Anzuzeigende Spalten festlegen
		$ph=popupMask($mtitle, '', '', 'dclpu', $puw, -1);
		echo($ph);
		// Selection
		$dbck=get_text(254, '20');
		$dccd=explode(',', meldung(16, true));
		echo('<table width="100%" border="0" cellpadding="4" cellspacing="4">'.$nl);
		echo('	<tr>'.$nl);
		// Input/Select + Icons
		$op35='opacity:0.35;';
		$op95='opacity:0.95;';
		fxSession('s','l');
		$cvasval=getColumnSelects($mn,$ids,$fs,$def,$all);
//fxDebug($cvasval, '$cvasval');
		$cva=$cvasval['cva'];
		$sval=$cvasval['sval'];
		$mss=explode('_',$sval);
//fxDebug($mss,'$mss');
		if($mss[0] === 'x')
		{
			$rctxt_dis=false;
			$rcsel_dis='hide';
			$rcres_op=$op95;
			$rcsav_op=$op95;
			$rcdel_op=$op35;
		}
		else
		{
			$rctxt_dis='hide';
			$rcsel_dis=false;
			$rcres_op=$op35;
			$rcsav_op=$op35;
			$rcdel_op=$op35;
			if(strlen($mss[0]) > 1)
			{
				$msp=explode('-',$mss[0]);
				$pid=(int)$msp[1];
				if($GLOBALS['fxpglobals']['person'] == $pid)
					$rcdel_op=$op95;
			}
		}
		$acst=' style="background:#f0f0f2;border-bottom:1px solid #888888;border-right:1px solid #888888;box-shadow:2px 2px 4px rgba(0,0,0, 0.33);"';
		echo('		<td height="40" align="left" nowrap'.$acst.'>'.$nl);
		echo('			<div style="float:left;margin-left:12px;">'.$nl);
		// ...Input
		$txt=$_POST['rctxt'];
		if(!$txt)
			$txt = '?';
		echo('			<div style="float:left;">'.fieldText('rctxt_'.$um,$txt,$isw+1,64,'left',false,$rctxt_dis).'</div>'.$nl);
		// ...Select
		$GLOBALS['rcsel_'.$um.'_ds_array']=$cva;
		echo('			<div style="float:left;">'.fieldSelect('rcsel_'.$um,$sval,$isw,0,false,$rcsel_dis).'</div>'.$nl);
		fxSession('s','s');
		// ...Icons
		echo('			&nbsp;&nbsp;');
		echo('			<img id="rcres_'.$um.'" src="'.$GLOBALS['gfxpath'].'icrs.png" align="top" tooltip="'.$dbck.'" style="margin-top:4px;cursor:default;'.$rcres_op.'">');
		echo('			<img id="rcsav_'.$um.'" src="'.$GLOBALS['gfxpath'].'icsv.png" align="top" tooltip="'.$dccd[1].'" style="margin-top:4px;cursor:default;'.$rcsav_op.'">');
		echo('			<img id="rcdel_'.$um.'" src="'.$GLOBALS['gfxpath'].'icde.png" align="top" tooltip="'.$dccd[3].'" style="margin-top:4px;cursor:default;'.$rcdel_op.'">');
		echo('			&nbsp;&nbsp;'.$nl);
		echo('			</div>'.$nl);
		echo('		</td>'.$nl);
		// Action: Default, All + Reverse
		echo('		<td height="40" align="right" nowrap'.$acst.'>'.$nl);
		echo('			&nbsp;&nbsp;'.fieldSubmit('rcdef_'.$um,'Default','sub',110).'&nbsp;'.fieldSubmit('rcall_'.$um,meldung(1422, true),'sub',110).'&nbsp;'.fieldSubmit('rcrev_'.$um,meldung(566, true),'sub',110).'&nbsp;&nbsp;'.$nl);
		echo('		</td>'.$nl);
		echo('	</tr>'.$nl);
		echo('</table><br>'.$nl);

		// Fields
		$felder='';
		if(sizeof($farr))
		{
//fxDebug($farr, '$farr');
			$felder='<table align="center" border="0" cellpadding="0" cellspacing="4">'.$nl;
			$fas=sizeof($farr);
			foreach($farr[0] as $fzn => $fzi)
			{
				$felder .= '<tr>'.$nl;
				for($fsn=0; $fsn<$fas; $fsn++)
				{
					if(isset($farr[$fsn][$fzn]))
						$felder .= $farr[$fsn][$fzn].$nl;
					else
						$felder .= '<td colspan="3"></td>'.$nl;
				}
				$felder .= '</tr>'.$nl;
			}
			$felder .= '</table>'.$nl;
		}
		echo($felder);

		echo('</div>'.$nl);
	}

	// Action
	else
	{
//echo('$mn='.$mn.', $action='.$action.', $fs='.$fs.', P[rcsel]='.$_POST['rcsel'].', P[rctxt]='.$_POST['rctxt']);

		$success=false;

		$rcsel_text	= trim($_POST['rctxt']);
		$rcsel_org	= '';
		$rcsel_time	= $GLOBALS['datetime'];
		$rcsel_pers	= 0;
		$rcsel_val	= '';
		$rcsel_ids	= '';
		$rcsel_idar	= array();

		if(strlen($fs))
		{
			$rcsel_org = trim($_POST['rcsel']);
			$ulpos = strpos($rcsel_org, '_');
			if($ulpos !== false)
			{
				if($ulpos > 15)
				{
					$rcsel_time	= substr($rcsel_org,0,14);
					$rcsel_pers	= (int)substr($rcsel_org, 15, $ulpos-15);
				}
				$rcsel_val	= substr($rcsel_org, $ulpos+1);
				if(strlen($rcsel_val))
				{
					for($c=0; $c<strlen($rcsel_val); $c++)
					{
						$ac = substr($rcsel_val,$c,1);
						if(($ac != '1') && ($ac != '3'))
							continue;

						$id = (int)$ids[$c]['id'];
						if($rcsel_idar[$id])
							continue;

						if(strlen($rcsel_ids))
							$rcsel_ids .= '_';
						$rcsel_ids .= $id;
						$rcsel_idar[$id] = true;
					}
				}
			}
		}
//echo($nl.'org="'.$rcsel_org.'"'.$nl.'time=['.$rcsel_time.'], pers=['.$rcsel_pers.'], val=['.$rcsel_val.']'.$nl.'ids=['.$rcsel_ids.']'.$nl.'text="'.$rcsel_text.'"'.$nl);

		// Save
		if(($action == 'sav') && strlen($rcsel_text) && strlen($rcsel_ids))
		{
//echo($nl.'SAVE:'.$nl);
			$nam = convert_string(fxReplaceSpecialCharacters($rcsel_text), 'todb');

			$sql  = "DELETE FROM reportspalten";
			$sql .= " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id=".$GLOBALS['fxpglobals']['person']." AND trans_id=".$GLOBALS['fxpglobals']['tr'];
			$sql .= " AND nummer=".$mn." AND einstellungswert='".$rcsel_ids."'";
//echo($sql.$nl.$nl);
			$ier = db_x($sql);
			if(!$ier)
			{
				$sql  = "INSERT INTO reportspalten";
				$sql .= " (mandanten_id, personen_id, trans_id, id_sprache,";
				$sql .= " nummer, feldname, einstellungswert)";
				$sql .= " VALUES";
				$sql .= " (".$GLOBALS['fxpglobals']['client'].", ".$GLOBALS['fxpglobals']['person'].", ".$GLOBALS['fxpglobals']['tr'].", ".$GLOBALS['fxpglobals']['lang'].",";
				$sql .= " ".$mn.", '".$nam."', '".$rcsel_ids."')";
//echo($sql.$nl.$nl);
				$ier = db_x($sql);
			}
			if(!$ier)
				$success=true;
		}

		// Delete
		if(($action == 'del') && (strlen($rcsel_time) == 14))
		{
//echo($nl.'DELETE:'.$nl);
			$where  = " WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id=".$GLOBALS['fxpglobals']['person']." AND trans_id=".$GLOBALS['fxpglobals']['tr'];
			$where .= " AND nummer=".$mn." AND zeitstempel='".$rcsel_time."'";

			$chk = "SELECT feldname FROM reportspalten".$where;
//echo($chk.$nl.$nl);
			$fname = trim(db_value($chk));
//echo('$fname='.$fname.$nl);
			if(strlen($fname))
			{
				$sql = "DELETE FROM reportspalten".$where;
//echo($sql.$nl.$nl);
				$ier = db_x($sql);
				if(!$ier)
					$success=true;
			}
			else
				$success=true;
		}

		if($success)
		{
			fxSession('s','l');

			// Unset old values
			fxSession('s','u', 'rcmain_'.$mn);
			fxSession('s','u', 'rcsel_'.$mn);

			// Set new values
			$cvasval=getColumnSelects($mn,$ids,$fs,$def,$all);
			fxSession('s','p', 'rcmain_'.$mn, $cvasval['cva']);
			fxSession('s','p', 'rcsel_'.$mn, $cvasval['cva']);

			fxSession('s','s');

			echo('OK');
		}
		else
			echo('KO');
	}
}
else
{
	if($_GET['lng'] == 1)
	{
		$txt1='Cookie nicht mehr g&uuml;ltig!';
		$txt2='Bitte aus- und wieder einloggen.';
	}
	else
	{
		$txt1='Cookie not valid anymore!';
		$txt2='Please logout and relogin again.';
	}
	echo(popupMessage($txt1.'<br><br><font class="black">'.$txt2.'</font>', 'red'));
}
?>