<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : personen_kompakt_pers_db.inc                                 //
// Version     : 21.1                                                         //
// Begin       : 2020-10-07                                                   //
// Last Change : 2020-10-07                                                   //
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
 * PF 166: Client etc. - Person database action preparations (will be called as an alternative program function from other person types as well)
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.1
 */

// Don't display database errors
$db_noerror_old=$db_noerror;
$db_noerror=true;

// Prepare person sql array
$dbz_arr['pers']=array();
$dbz=0;

// Get all submitted fields (except in deletion mode)
$dbfa=array();
if($GLOBALS['fxpglobals']['action'] != FXP_DELETE)
{
	foreach($ppta as $ppf => $ppp)
	{
		// Not hidden
		if(strpos($ppp,'#') === false)
		{
			// Field type: s=Select, t=Text, d=Date + c=Checkbox
			$ft=substr($ppp,0,1);

			if($ft == 's')		// Select
			{
				if($ppf == 'Elter')
					$dbfa[$ppf]=$_ppid;
				else
					$dbfa[$ppf]=(int)$_POST[$ppf];
			}
			else if($ft == 'c')	// Checkbox
			{
				if($_POST[$ppf])
					$dbfa[$ppf]=1;
				else
					$dbfa[$ppf]=0;
			}
			else				// Text or Date
				$dbfa[$ppf]=db_convert_string($_POST[$ppf]);
		}
	}

	// Special: Empty superior submitted, then set superior to client
	if(isset($dbfa['Elter']) && !$dbfa['Elter'])
		$dbfa['Elter']=(int)db_value("SELECT personen_id FROM personen WHERE personentyp=".FXP_PT_CLIENT." AND mandanten_id=".$_cid);
}
fxDebug($dbfa,'$dbfa', 2);

// Create
if($GLOBALS['fxpglobals']['action'] == FXP_CREATE)
{
	$_pid=id_max('Personen', 'Personen_ID');
	$_POST['nv_text5']=$_pid;	// for Cost Accounting

	// Person
	$cols="";
	$vals="";
	$dca=array('mandanten_id', 'personen_id', 'personentyp', 'pname', 'vorname', 'personen_initialen', 'name_zusatz', 'anrede', 'perstitel', 'pposition', 'elter', 'geschlecht', 'abt_id', 'inaktiv', 'gueltig_von', 'gueltig_bis');
	foreach($dca as $dcc => $dcn)
	{
		if($dcc)
		{
			$cols .= ", ";
			$vals .= ", ";
		}

		$cols .= $dcn;

		$val="NULL";
		if($dcn == 'mandanten_id')
			$val=$_cid;
		else if($dcn == 'personen_id')
			$val=$_pid;
		else if($dcn == 'personentyp')
			$val=$_ptype;
		else
		{
			foreach($dbfa as $fcn => $fv)
			{
				if(strtolower($fcn) == $dcn)
				{
					$val=$fv;
					break;
				}
			}
		}
		$vals .= $val;
	}
	$sql="INSERT INTO personen (".$cols.") VALUES (".$vals.")";
	$dbz_arr['pers'][$dbz++]=array('sql'=>$sql, 'typ'=>'insert');
}

// Change
else if(($GLOBALS['fxpglobals']['action'] == FXP_CHANGE) && $_pid && $_cid)
{
	$uc=0;
	$sql="UPDATE personen SET ";
	foreach($dbfa as $dbc => $dbv)
	{
		if($uc)
			$sql .= ", ";
		$sql .= $dbc."=".$dbv;
		$uc++;
	}
	$sql .= " WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
	$dbz_arr['pers'][$dbz++]=array('sql'=>$sql, 'typ'=>'update');
}

// Delete
else if(($GLOBALS['fxpglobals']['action'] == FXP_DELETE) && $_POST['Button_PGLoeschen'] && $_pid && $_cid)
{
	// Delete everything from this existing tables
	$dta=array('benutzer', 'kommunikation', 'ma_art_faehigkeiten', 'madaten');
	foreach($dta as $tabname)
	{
		if(db_check($tabname, 't'))
		{
			$chk=db_value("SELECT personen_id FROM ".$tabname." WHERE personen_id=".$_pid." AND mandanten_id=".$_cid);
			if($chk)
			{
				$sql="DELETE FROM ".$tabname." WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
				$dbz_arr['pers'][$dbz++]=array('sql'=>$sql, 'typ'=>'delete');
			}
		}
	}

	// Delete existing start and monthly carry-over values
	if(db_check('zeitdaten', 't'))
	{
		$chk=db_value("SELECT personen_id FROM zeitdaten WHERE zeitart IN (".FXP_TE_CARRY_MONTH.",".FXP_TE_CARRY_START.") AND personen_id=".$_pid." AND mandanten_id=".$_cid);
		if($chk)
		{
			$sql="DELETE FROM zeitdaten WHERE zeitart IN (104,105) AND personen_id=".$_pid." AND mandanten_id=".$_cid;
			$dbz_arr['pers'][$dbz++]=array('sql'=>$sql, 'typ'=>'delete');
		}
	}

	// Delete everything from this special tables (with check if data is not used in other tables)
	$dta=array('adressen'=>"adressen_id", 'artikel'=>"artikel_id", 'personen'=>"personen_id");
	foreach($dta as $tabname => $tabcolumn)
	{
		if(db_check($tabname, 't'))
		{
			$sql="SELECT ".$tabcolumn." FROM ".$tabname." WHERE personen_id=".$_pid." AND mandanten_id=".$_cid;
			$arr=db_values($sql, '*');
			if(fxIsArray($arr))
			{
				foreach($arr as $a)
				{
					$sid=$a[strtolower($tabcolumn)];
					$sql="DELETE FROM ".$tabname." WHERE ".$tabcolumn."=".$sid." AND mandanten_id=".$_cid;

					$dbz_arr['pers'][$dbz++]=array('sql'=>$sql, 'ckd'=>array('tab'=>$tabname, 'fld'=>$tabcolumn, 'val'=>$sid), 'typ'=>'delete');
				}
			}
		}
	}
}

// Check person's contract data?
if((($_ptype == FXP_PT_EMPLOYEE) || ($_ptype == FXP_PT_CONTRACTOR)) && $_cid && $_pid && (($GLOBALS['fxpglobals']['action'] == FXP_CREATE) || ($GLOBALS['fxpglobals']['action'] == FXP_CHANGE)))
{
	$cda=fxf_checkPersonContractData($_pid, $_cid, $_ptype);
	if(sizeof($cda))
	{
		foreach($cda as $ca)
			$dbz_arr['pers'][$dbz++]=$ca;
	}
}
//$err=true;

// Reset error messages
if(!$db_noerror_old)
	unset($db_noerror);
?>