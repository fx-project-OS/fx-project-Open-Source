<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : delpic.inc                                                   //
// Version     : 24.1                                                         //
// Begin       : 2020-10-07                                                   //
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
 * Delete person or material pictures
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 24.1
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

// GET
// ...program function
$tr=0;
if(fxIsArray($_GET) && isset($_GET['tr']))
	$tr=(int)$_GET['tr'];
// ...language
$lang=$GLOBALS['fxpglobals']['lang'];
if(fxIsArray($_GET) && isset($_GET['lang']))
	$lang=(int)$_GET['lang'];

// POST
// ...form
$frm='';
if(is_array($_POST) && isset($_POST['frm']))
	$frm=trim((string)$_POST['frm']);
// ...parameter array
$parr=false;
if(is_array($_POST) && isset($_POST['param']))
{
	$param=trim((string)$_POST['param']);
	if(strlen((string)$param))
		$parr=explode('|',$param);
}

// Valid?
$rc='100';
$rp='';
$rm='';
if(fxIsArray($parr) && (sizeof($parr) >= 6))
{
	$pm=strtoupper(substr((string)$parr[0],0,3));	// PER or MAT
	$ci=(int)$parr[1];						// Client ID
	$am=(int)$parr[2];						// Action
	$id=(int)$parr[3];						// Person or Material ID
	$sx=(int)$parr[4];						// Sex
	$nm=trim((string)$parr[5]);						// Name

	if((($pm == 'PER') || ($pm == 'MAT')) && ($ci > 0) && ($id > 0))
	{
		// Create client person/material folder
		$cdir=$GLOBALS['datpath'].$ci.'/';
		$cpdir=$cdir.$pm.'/';
		$cerr=fxCreatePath($cpdir);
//$rm .= '$tr='.$tr.', $lang='.$lang.'<hr />$pm='.$pm.'<br />$ci='.$ci.', $am='.$am.', $id='.$id.', $sx='.$sx.'<br />$nm='.$nm.'<hr />$cdir='.$cdir.'<br />$cpdir='.$cpdir.'<hr />';

		// Person/material folder exists
		if(!strlen((string)$cerr))
		{
			$dpc=0;

			// Delete person or material pictures in the corresponding person or material folder
			if($id > 0)
			{
				$cpa=array($cpdir.$id.'_src.png', $cpdir.$id.'_256.png', $cpdir.$id.'_128.png', $cpdir.$id.'_100.png', $cpdir.$id.'_ico.png');
				foreach($cpa as $cpf)
				{
					if(file_exists($cpf))
					{
//$rm .= 'DELETE: '.$cpf.'<br />';
						@chmod($cpf, 0755);
						@unlink($cpf);

						$dpc++;
					}
				}
			}
//$rm .= '<hr />';

			// Default icon
			$rp='';
			if($pm == 'PER')
			{
				if($sx == 8)		// ...male
					$ici='usm';
				else if($sx == 9)	// ...female
					$ici='usf';
				else
				{
					$tia=array(164=>'cst', 'grp', 'clt', 'wrk', 'ctr', 'cmp', 'cst', 'ctc', 'usm', 'grp', 'par');
					if(isset($tia[$tr]))
					{
						$ici=$tia[$tr];
						if(($ici == 'wrk') || ($ici == 'usm'))	// Select male or female icons randomly
						{
							$r=rand(1,1000);
							if($r > 500)
								$ici=substr((string)$ici,0,2).'f';
						}
					}
					else
						$ici='tmf';
				}
				$rp=$GLOBALS['gfxpath'].$ici.'_b_100x100.png';
//$rm .= '$rp='.$rp.'<hr />';
			}
			else
				$rp=$GLOBALS['gfxpath'].'mat_b_20x20.png';

			$rc='001';
			if($lang == 1)
				$rm .= 'Das Bild wurde erfolgreich gelöscht.';
			else
				$rm .= 'The picture has been deleted successfully.';
		}
		else
		{
			if($lang == 1)
				$rm .= 'Das Bilderverzeichnis &quot;<b>'.$pm.'</b>&quot; für den Mandanten existiert nicht und konnte auch nicht erstellt werden!';
			else
				$rm .= 'The picture folder &quot;<b>'.$pm.'</b>&quot; doesn\'t exist and couldn\'t be created!';
		}
	}
	else
	{
		if($lang == 1)
			$rm .= 'Ungültige Übergabeparameter!';
		else
			$rm .= 'Invalid submit parameters!';
	}
}
else
{
	if($lang == 1)
		$rm .= 'Ungültige Übergabeparameteranzahl!';
	else
		$rm .= 'Invalid submit parameters amount!';
}

echo($rc.'|'.$rp.'|'.$rm);
?>