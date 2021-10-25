<?php
////////////////////////////////////////////////////////////////////////////////
// File name   : head.inc                                                     //
// Version     : 21.2                                                         //
// Begin       : 2020-08-20                                                   //
// Last Change : 2021-09-30                                                   //
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
 * - Establish a database connection, and
 *   + Create configuration file, if not found
 *   + Update software, if necessary
 *   + Patch software, if necessary
 *
 * @author FeRox Management Consulting GmbH & Co. KG, Adolf-Langer-Weg 11a, D-94036 Passau (Germany)
 * @version 21.2
 */

$GLOBALS['__loaded_'.basename(__FILE__)]=true;


////////////////////////////////////////////////////////////////////////////////
// VARIABLES
////////////////////////////////////////////////////////////////////////////////
$db_noerror=true;		// Don't display any database errors
$in_kopf_v=true;		// Mark this as the "head.inc"

if(isset($GLOBALS['fxpglobals']['project']))
	$GLOBALS['fxpglobals']['project']=(int)$GLOBALS['fxpglobals']['project'];
else
	$GLOBALS['fxpglobals']['project']=0;


////////////////////////////////////////////////////////////////////////////////
// INITIALIZE DATABASE CONNECTION
////////////////////////////////////////////////////////////////////////////////
$dberrorlog='';
$dberror=db_ini();

// ...ini	-> No ini file found -> Initialize
if($dberror === 'ini')
{
	$err=true;
	return;
}
// ...reini	-> Entries missing -> Re-initialize
else if($dberror === 'reini')
{
	$req_fn=fxf_fn_reqFilename('fxp_ini');
	if(strlen($req_fn))
	{
		require($req_fn);
		$dberror=db_ini();
		if($dberror === 'reini')
			return;
	}
	else
		return;
}

////////////////////////////////////////////////////////////////////////////////
// INSTALLATION/UPDATE?
////////////////////////////////////////////////////////////////////////////////
// ...inst	-> No data -> Installation
// ...upd	-> Newer version found -> Update
$quit=false;
if(($dberror === 'inst') || ($dberror === 'upd'))
{
	if($dberror === 'inst')			// Installation
		$tpath='Installation';
	else							// Update
		$tpath='Update';

	$req_fn=fxf_fn_reqFilename('db_install');
	if(strlen($req_fn))
		require($req_fn);
	if($quit)
		return;
}
else if(fxIsArray($dberror))		// Error
{
	$tpath=$dberror['tpath'];
	leave(4,$dberror['title'],$dberror['text']);

	$err=true;
	return;
}


////////////////////////////////////////////////////////////////////////////////
// PATCH?
////////////////////////////////////////////////////////////////////////////////
$perr=dbExecutePatch();
if($quit)
	return;


////////////////////////////////////////////////////////////////////////////////
// CLIENT 1?
////////////////////////////////////////////////////////////////////////////////
$req_fn=fxf_fn_reqFilename('client1');
if(strlen($req_fn))
	require($req_fn);
if($quit)
	return;


////////////////////////////////////////////////////////////////////////////////
// CLIENT TESTDATA?
////////////////////////////////////////////////////////////////////////////////
$req_fn=fxf_fn_reqFilename('client_testdata');
if(strlen($req_fn))
	require($req_fn);
if($quit)
	return;

////////////////////////////////////////////////////////////////////////////////
// SYSADMIN?
////////////////////////////////////////////////////////////////////////////////
unset($db_noerror);
$get_daten=false;
if($_POST['Button_OK'] && $_POST['sali'])
{
	$msl=0;
	if(!$_POST['Mandanten_ID'])
		$_POST['Mandanten_ID']=1;
	$misplit=explode('-', trim($_POST['Mandanten_ID']));

	$GLOBALS['fxpglobals']['person']=0;
	$GLOBALS['fxpglobals']['user']=1;
	$GLOBALS['fxpglobals']['suser']=1;
	$GLOBALS['fxpglobals']['client']=(int)$misplit[0];

	$_POST['Name_Mandant']=(string)$GLOBALS['fxpglobals']['client'];
	$_POST['Name_Benutzer']='fxp';

	$Personen_ID=0;

	// Change Sysadmin to selected person
	if(sizeof($misplit) > 1)
	{
		$GLOBALS['fxpglobals']['person']=(int)$misplit[1];
		$GLOBALS['fxpglobals']['user']=(int)$misplit[2];
		$Personen_ID=$GLOBALS['fxpglobals']['person'];
		$_POST['Name_Benutzer']=db_value("SELECT name_benutzer FROM benutzer WHERE benutzer_id=".$GLOBALS['fxpglobals']['user']." AND personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$_POST['Name_Mandant']);
		$get_daten=true;
	}
}

// Get all history tables
if(!$GLOBALS['fxpglobals']['historytables'])
	get_history_tables();

$content='';

// Decode submitted password
$pst_pw='';
if(isset($_POST['Passwort']) && strlen($_POST['Passwort']))
	$pst_pw=fxDecodeValue($_POST['Passwort']);

if(!isset($GLOBALS['fxptdata']['logincounter']) || (isset($_GET['uid']) && ($_GET['uid'] == 'login')) || !$lcnt)
{
	get_history_tables();
	$GLOBALS['fxptdata']['logincounter']=0;
}
if($_POST['Button_Login'])
	$GLOBALS['fxptdata']['logincounter']++;

// Get user data
if(!$err && ($get_daten || (isset($_POST['Button_Login']) && isset($_POST['Name_Mandant']) && isset($_POST['Name_Benutzer']) && isset($_POST['Passwort']))))
{
	if(!$_POST['Name_Mandant'])
		$_POST['Name_Mandant']='1';

	$emmid=(int)$_POST['Name_Mandant'];

	$email_adata=db_values("SELECT personen_id, e_mail_adresse FROM benutzer WHERE benutzer_id=1 AND mandanten_id=".$emmid);
	$pid_admin=(int)$email_adata['personen_id'];
	$email_admin=$email_adata['e_mail_adresse'];
	if(!$email_admin)
		$email_admin=$GLOBALS['fxpglobals']['dbparam']['emailfrom'];

	$email_adata1=db_values("SELECT personen_id, e_mail_adresse FROM benutzer WHERE benutzer_id=1 AND mandanten_id=1");
	$pid_admin1	=(int)$email_adata1['personen_id'];
	$email_admin1=$email_adata1['e_mail_adresse'];
	if(!$email_admin1)
		$email_admin1=$GLOBALS['fxpglobals']['dbparam']['emailfrom'];

	// Login as Sysadmin if correct credentials and allowed in setting in fx_project.ini?
	$benutzerdaten=array();
	if($GLOBALS['fxpglobals']['dbparam']['sysadmin'] && (strtolower($_POST['Name_Benutzer']) == 'sysadmin') && (strtolower($_POST['Name_Mandant']) == 'fxp') && ($pst_pw == substr($GLOBALS['fxpglobals']['dbparam']['oskey32'],(int)substr($GLOBALS['date'],6,2),8)))
	{
		$_POST['Name_Mandant']='fxp';
		$_POST['Name_Benutzer']='sysadmin';

		$benutzerdaten['mandanten_id']=0;
		$benutzerdaten['benutzer_id']=1;
		$benutzerdaten['personen_id']=0;
		$benutzerdaten['name_mandant']='fx-project';
		$benutzerdaten['name_benutzer']='sysadmin';
		$benutzerdaten['name_person']='Systemadministrator';
		$benutzerdaten['passwort']=fxf_ecPasswd($pst_pw, '', 0, 1);
		$benutzerdaten['e_mail_adresse']='dummy';
		$benutzerdaten['login_zaehler']=0;
		$benutzerdaten['status_benutzer']=0;
	}
	if($_POST['Name_Mandant'] != 'fxp')
		$_POST['Name_Mandant']=(string)((int)$_POST['Name_Mandant']);
	if(!fxIsArray($benutzerdaten))
	{
		$benutzerdaten=db_values("SELECT * FROM benutzer WHERE mandanten_id=".(int)$_POST['Name_Mandant']." AND name_benutzer='".strtolower($_POST['Name_Benutzer'])."'");
		if(fxIsArray($benutzerdaten))
		{
			$benutzerdaten['name_mandant']='?';
			$benutzerdaten['name_person']='?';
			$tmparr=db_values("SELECT personentyp,pname,vorname FROM personen WHERE mandanten_id=".(int)$benutzerdaten['mandanten_id']." AND (personentyp=274 OR personen_id=".(int)$benutzerdaten['personen_id'].")", '*');
//fxDebug($tmparr, 'user data addons', 0);
			if(fxIsArray($tmparr))
			{
				foreach($tmparr as $t)
				{
					$tpname=trim($t['pname']);
					$tvname=trim($t['vorname']);
					if(strlen($tvname))
						$tpname .= ', '.$tvname;

					if($t['personentyp'] == 274)
						$benutzerdaten['name_mandant']=$tpname;
					else
						$benutzerdaten['name_person']=$tpname;
				}
			}
		}
	}
//fxDebug($benutzerdaten,'$benutzerdaten', 0);

	// User found, check login data
	if(fxIsArray($benutzerdaten))
	{
		$GLOBALS['fxpglobals']['client']=(int)$benutzerdaten['mandanten_id'];
		$GLOBALS['fxpglobals']['person']=(int)$benutzerdaten['personen_id'];

		$GLOBALS['fxpglobals']['cname']=$benutzerdaten['name_mandant'];
		$GLOBALS['fxpglobals']['uname']=strtolower(trim($benutzerdaten['name_benutzer']));
		$GLOBALS['fxpglobals']['pname']=$benutzerdaten['name_person'];

		if(!isset($GLOBALS['fxptdata']['cperson']) || ($GLOBALS['fxptdata']['cperson'] != $GLOBALS['fxpglobals']['client'].'_'.$GLOBALS['fxpglobals']['person']))
			$GLOBALS['fxptdata']['logincounter']=1;

		$GLOBALS['fxptdata']['cperson']=$GLOBALS['fxpglobals']['client'].'_'.$GLOBALS['fxpglobals']['person'];

		// Current date
		$adate=substr($GLOBALS['datetime'],0,8);

		// Check if user is active
		if($benutzerdaten['status_benutzer'] == FXP_USER_INACTIVE) // FXP_USER_INACTIVE
		{
			// Der Benutzer "<b>$var1</b>" ist <u>nicht</u> mehr aktiviert!<br /><br />Falls dies irrtümlich geschehen ist oder Sie versuchen wollen,<br />sich erneut als "aktiver" Benutzer registrieren zu lassen,<br />dann wenden Sie sich bitte an Ihren <a href="mailto:$var2?subject=$var3">Administrator</a>!
			$err=true;
			$ema=array(2,meldung(1270),get_message('BS_Inaktiv', $_POST['Name_Benutzer']));
		}
		else if($benutzerdaten['status_benutzer'] == FXP_USER_LOCKED) // FXP_USER_LOCKED
		{
			// Der Benutzer "<b>$var1</b>" ist <u>gesperrt</u>!<br /><br />Falls die Sperrung aufgehoben werden soll kontaktieren Sie bitten Ihren <a href="mailto:$var2?subject=$var3">Administrator</a>!
			$err=true;
			$ema=array(2,meldung(1273),get_message('BS_Gesperrt', $_POST['Name_Benutzer']));
		}

		$valid_array=array();
		if(!$err)
		{
			// Adjust user login data
			$luser=(int)$benutzerdaten['benutzer_id'];

			$GLOBALS['fxpglobals']['settings']=benutzereinstellungen($luser);
			$GLOBALS['fxpglobals']['useradmin']=(int)$benutzerdaten['b_admin'];

			// Set language to user settings
			$GLOBALS['fxpglobals']['lang']=$GLOBALS['fxpglobals']['settings']['sprache'];

			$valid_array=array(0 => array("typ"=>0, "pname"=>$benutzerdaten['name_benutzer'], "vorname"=>'', "inaktiv"=>0, "gueltig_von"=>$benutzerdaten['gueltig_von'], "gueltig_bis"=>$benutzerdaten['gueltig_bis']));

			// Get assigned person data
			$sql="SELECT personen_id, personentyp, elter, pname, vorname, inaktiv, gueltig_von, gueltig_bis FROM personen WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
			$pers_data=db_values($sql);
//fxDebug($pers_data,$sql, 0);
			if(fxIsArray($pers_data))
				$valid_array[$pers_data['personen_id']]=array("typ"=>1, "pname"=>$pers_data['pname'], "vorname"=>$pers_data['vorname'], "inaktiv"=>$pers_data['inaktiv'], "gueltig_von"=>$pers_data['gueltig_von'], "gueltig_bis"=>$pers_data['gueltig_bis']);

			// Get parent data from contact persons: 620=Customer, 841=Contact + 2092=Contract Partner
			if(fxIsArray($pers_data) && $pers_data['elter'] && (($pers_data['personentyp'] == 620) || ($pers_data['personentyp'] == 841) || ($pers_data['personentyp'] == 2092)))
			{
				$sql="SELECT personen_id, pname, vorname, inaktiv, gueltig_von, gueltig_bis FROM personen WHERE personen_id=".$pers_data['elter']." AND mandanten_id=".$GLOBALS['fxpglobals']['client'];
				$elt_data=db_values($sql);
//fxDebug($elt_data,$sql, 0);
				if(fxIsArray($elt_data))
					$valid_array[$elt_data['personen_id']]=array("typ"=>2, "pname"=>$elt_data['pname'], "vorname"=>$elt_data['vorname'], "inaktiv"=>$elt_data['inaktiv'], "gueltig_von"=>$elt_data['gueltig_von'], "gueltig_bis"=>$elt_data['gueltig_bis']);
			}

			// Get client data
			$sql="SELECT personen_id, pname, vorname, inaktiv, gueltig_von, gueltig_bis FROM personen WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personentyp=274";
			$mand_data=db_values($sql);
//fxDebug($mand_data,$sql, 0);
			if(fxIsArray($mand_data))
				$valid_array[$mand_data['personen_id']]=array("typ"=>3, "pname"=>$GLOBALS['fxpglobals']['client'].': '.$mand_data['pname'], "vorname"=>'', "inaktiv"=>$mand_data['inaktiv'], "gueltig_von"=>$mand_data['gueltig_von'], "gueltig_bis"=>$mand_data['gueltig_bis']);
//fxDebug($valid_array,'$valid_array', 0);
		}

		// Check if login is allowed, i.e. user and assigned person are active and in allowed timeframe
		$abu=explode(',', meldung(2853, true));	// from,to,unlimited
		if(fxIsArray($valid_array))
		{
			foreach($valid_array as $vdata)
			{
				$vname=trim($vdata['pname']);
				if(strlen(trim($vdata['vorname'])))
					$vname=trim($vdata['vorname']).' '.$vname;

				$sdate=substr($vdata['gueltig_von'], 0, 8);
				$edate=substr($vdata['gueltig_bis'], 0, 8);
				if(strlen($sdate) && strlen($edate))
					$vframe=get_entry($sdate, 'datum').' - '.get_entry($edate, 'datum');
				else if(strlen($sdate))
					$vframe=$abu[0].' '.get_entry($sdate, 'datum');	// from
				else if(strlen($edate))
					$vframe=$abu[1].' '.get_entry($edate, 'datum');	// to
				else
					$vframe=$abu[2];	// unlimited

				// Inactive or not in allowed timefram?
				if($vdata['inaktiv'] || (strlen($sdate) && ($adate < $sdate)) || (strlen($edate) && ($adate > $edate)))
				{
					if(isset($GLOBALS['fxptdata']['logincounter']) && ($GLOBALS['fxptdata']['logincounter'] > 0))
						--$GLOBALS['fxptdata']['logincounter'];

					$salarr=explode(',', meldung(2852, true));	// Ihr Benutzer,Ihre zugeordnete Person,Ihre übergeordnete Firma,Ihr kompletter Mandant
					$titarr=explode(',', meldung(2854, true));	// Login inaktiv,Zeitraum noch nicht gültig,Zeitraum nicht mehr gültig

					// Login inactive?
					if($vdata['inaktiv'])
					{
						$titel=strtoupper($titarr[0]);
						$fehler=get_message('Login_Inaktiv', $salarr[$vdata['typ']], $vname);
					}

					// Timeframe: not yet valid
					else if(strlen($sdate) && ($adate < $sdate))
					{
						$titel=strtoupper($titarr[1]);
						$fehler=get_message('Zeitraum_noch_nicht_gueltig', $salarr[$vdata['typ']], $vname, $vframe);
					}

					// Timeframe: not valid anymore
					else if(strlen($edate) && ($adate > $edate))
					{
						$titel=strtoupper($titarr[2]);
						$fehler=get_message('Zeitraum_nicht_mehr_gueltig', $salarr[$vdata['typ']], $vname, $vframe);
					}
					$err=true;
					$ema=array(2,$titel,$fehler);
				}
			}
		}

		// Lock user...
		$lock_user=true;
		// ...except administrators
		if($luser < 2)
			$lock_user=false;
		if(($GLOBALS['fxpglobals']['client'] < 2) || !$GLOBALS['fxpglobals']['person'] || !$benutzerdaten['benutzer_id'])
			$lock_user=false;

		// Check password
		$GLOBALS['_updpw']='';
		$cpw=fxf_ecPasswd($pst_pw, $benutzerdaten['passwort'], $benutzerdaten['mandanten_id'], $benutzerdaten['benutzer_id']);
//$cpw=true;

		if(!$_POST['sali'] && !$cpw)
		{
			$err=true;

			// Max. login attempts reached?
			if($GLOBALS['fxptdata']['logincounter'] >= $GLOBALS['fxpglobals']['settings']['logins'])
			{
				if($lock_user)
				{
					$sql="UPDATE benutzer SET status_benutzer=".FXP_USER_LOCKED." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id=".$GLOBALS['fxpglobals']['person']." AND benutzer_id=".$benutzerdaten['benutzer_id'];
					@db_x($sql);
				}
			}

			// Wrong login credentials?
			$fehler=get_message('BS_Anmeldefehler');
			if($GLOBALS['fxptdata']['logincounter'] >= $GLOBALS['fxpglobals']['settings']['logins'])
			{
				if($lock_user)
					$fehler .= get_message('BS_BSperren');
				$fehler .= get_message('BS_Loginfehler', $GLOBALS['fxptdata']['logincounter']);
			}
			$ema=array(3,meldung(1232),$fehler);
		}

		// User login successfull, increase login counter and check for updates
		if(!$err)
		{
			$GLOBALS['fxpglobals']['user']=$luser;
			if(!$GLOBALS['fxpglobals']['client'])
				$GLOBALS['fxpglobals']['user']=0;
			else
			{
				$gp_filename=fxSessionFile('g.inc',$GLOBALS['fxpglobals']['client'],$GLOBALS['fxpglobals']['person'],true);
				if(file_exists($gp_filename))
				{
					$sa=array();
					require($gp_filename);
					if(sizeof($sa))
					{
						$sgia=array('client','user','suser','person','cname','uname','pname','locstoid','sesstoid','main_path','ssl','jscoded','swidth','sheight','browser','browservers','browsermobile','dbparam','historytables','settings','useradmin','persdat','menudate');
						foreach($sa as $sak => $sav)
						{
							$sgadd=1;
							foreach($sgia as $sgak)
							{
								if($sak == $sgak)
								{
									$sgadd=0;
									break;
								}
							}
							if($sgadd)
								$GLOBALS['fxpglobals'][$sak]=$sav;
						}
					}
				}
			}
			// Increase login counter without history
			if($GLOBALS['fxpglobals']['client'] && $GLOBALS['fxpglobals']['person'] && $GLOBALS['fxpglobals']['user'])
			{
				$GLOBALS['fxptdata']['logincounter']=0;
				$logins=$benutzerdaten['login_zaehler']+1;
				$sql="UPDATE benutzer SET login_zaehler=".$logins.$GLOBALS['_updpw']." WHERE mandanten_id=".$GLOBALS['fxpglobals']['client']." AND personen_id=".$GLOBALS['fxpglobals']['person']." AND benutzer_id=".$GLOBALS['fxpglobals']['user'];
				@db_x($sql, 0, false, true, false);

				$_notopclose=true;

				fxSession('t0','s');
			}
		}
	}

	// User does not exist?
	else
	{
		$GLOBALS['fxpglobals']['client']=(int)$_POST['Name_Mandant'];
		$err=true;
		// Wrong login credentials
		$fehler=get_message('BS_Anmeldefehler');
		if(!$_POST['sali'] && ($GLOBALS['fxptdata']['logincounter'] >= 5) && $lock_user)
			$fehler .= get_message('BS_Loginfehler', $GLOBALS['fxptdata']['logincounter']);
		$ema=array(3,meldung(1232),$fehler);
	}

	if(!$err)
	{
		if($GLOBALS['fxpglobals']['person'])
			$GLOBALS['fxpglobals']['persdat']=db_values("SELECT pname, vorname, personentyp, pposition, elter, abt_id, geschlecht FROM personen WHERE personen_id=".$GLOBALS['fxpglobals']['person']." AND mandanten_id=".$GLOBALS['fxpglobals']['client']);
		if(fxIsArray($GLOBALS['fxpglobals']['persdat']))
		{
			$_pposition=$GLOBALS['fxpglobals']['persdat']['pposition'];
			$GLOBALS['fxpglobals']['persdat']['name1']=$GLOBALS['fxpglobals']['persdat']['pname'];
			$GLOBALS['fxpglobals']['persdat']['name2']=$GLOBALS['fxpglobals']['persdat']['pname'];
			if(strlen($GLOBALS['fxpglobals']['persdat']['vorname']))
			{
				$GLOBALS['fxpglobals']['persdat']['name1']  = $GLOBALS['fxpglobals']['persdat']['vorname'].' '.$GLOBALS['fxpglobals']['persdat']['name1'];
				$GLOBALS['fxpglobals']['persdat']['name2'] .= ', '.$GLOBALS['fxpglobals']['persdat']['vorname'];
			}
//fxDebug($GLOBALS['fxpglobals']['persdat'],'persdat', 0);
		}
	}

	// Login as Sysadmin - Display client selection
	if(!$err && ($GLOBALS['fxpglobals']['client'] == 0) && (strtolower($_POST['Name_Benutzer']) == 'sysadmin') && !$jump)
	{
		setForm('clientselect_3');
		$maskenfeldwerte=array();

		// Sysadmin (Checkbox)
		$maskenfeldwerte['_sysadmin']=1;

		// Client (Select)
		$maskenfeldwerte['mandanten_id']=0;

		$maske156_sf=true;
		$content .= maske(156);

		// Hidden
		$content .= fieldHidden('sali',1);

		std_screen($prehtml, $content);
		return;
	}

	// Get filter settings and person rights
	if(!$err && (($uid == 'Button_Login') || !is_array($GLOBALS['fxpglobals']['filter'])))
	{
		$flt_arr=getFilters(false);
		if(strlen($flt_arr['setting']))
		{
			eval($flt_arr['setting']);
		}
		else
		{
			// Create default filter settings
			$GLOBALS['fxpglobals']['filter']=array(
				'fname'	=> "",
				'fdesc'	=> "",

				// ...Person
				'fperson_inaktiv'		=> '',			// Personenfilter aktiv
				'pname'					=> '',			// Suchtext
				'projektteam_id'		=> 0,			// Projekt-Team
				'vorgesetzter'			=> 0,			// Vorgesetzter
				'abt_id'				=> 0,			// Abteilung
				'person_alias'			=> array(0),	// Lieferant
				'mafaehigkeit_id'		=> array(0),	// Fähigkeit
				'personentyp_1372'		=> 1,			// Kontakt (CRM)
				'personentyp_841'		=> 1,			// Kontakt (CRM) - Ansprechpartner
				'personentyp_276'		=> 1,			// Auftraggeber
				'personentyp_620'		=> 1,			// Auftraggeber - Ansprechpartner
				'personentyp_2091'		=> 1,			// Lieferant
				'personentyp_2092'		=> 1,			// Lieferant - Ansprechpartner
				'personentyp_274'		=> 1,			// Mandant
				'personentyp_2090'		=> 1,			// Mandant - Ansprechpartner
				'personentyp_840'		=> 1,			// Mitarbeiter (extern)
				'personentyp_275'		=> 1,			// Mitarbeiter (intern)
				'inaktiv'				=> '',			// inaktiv

				// ...Time
				'fzeit_inaktiv'			=> '',			// Zeitfilter aktiv
				'ist_beg_dtm'			=> '',			// Startdatum
				'ist_end_dtm'			=> '',			// Enddatum

				// ...Project
				'fprojekt_inaktiv'		=> '',			// Projektfilter aktiv
				'projektname'			=> '',			// Suchtext
				'proj_menu_von'			=> '',			// Projekt vom
				'proj_menu_bis'			=> '',			// Projekt bis
				'mutterprojekt_id'		=> array(0),	// Mutterprojekt
				'kunde'					=> array(0),	// Auftraggeber
				'projektmanager'		=> array(0),	// Projektmanager
				'projektleiter'			=> array(0),	// Projektleiter
				'kategorie_id'			=> array(0),	// Prj.-Kategorie
				'projektstatus_297'		=> 1,			// Projekt-Status: geplant
				'projektstatus_300'		=> 1,			// Projekt-Status: aktiv
				'projektstatus_299'		=> 0,			// Projekt-Status: inaktiv
				'projektstatus_301'		=> 0,			// Projekt-Status: abgeschl.
				'prioritaet_445'		=> 1,			// Priorität: niedrig
				'prioritaet_446'		=> 1,			// Priorität: mittel
				'prioritaet_447'		=> 1,			// Priorität: hoch
				'prioritaet_1283'		=> 1,			// Priorität: sofort
				'aufgaben'				=> 1,			// Auch Aufgaben anzeigen
				'projekte_archiviert'	=> 1,			// Archivierte Projekte ausblenden
				'nurauftrag'			=> ''			// Nur Auftragsvorgänge anzeigen
			);
		}

		// Special settings
		if(($GLOBALS['fxpglobals']['persdat']['personentyp'] == 276) || ($GLOBALS['fxpglobals']['persdat']['personentyp'] == 620))		// 276=Auftraggeber oder 620=Auftraggeber - Ansprechpartner
		{
			if($GLOBALS['fxpglobals']['persdat']['elter'])
				$GLOBALS['fxpglobals']['filter']['kunde'][0]=$GLOBALS['fxpglobals']['persdat']['elter'];
			else
				$GLOBALS['fxpglobals']['filter']['kunde'][0]=$GLOBALS['fxpglobals']['person'];
		}
		else if(($GLOBALS['fxpglobals']['persdat']['personentyp'] == 2091) || ($GLOBALS['fxpglobals']['persdat']['personentyp'] == 2092))	// 2091=Lieferant oder 2092=Lieferant - Ansprechpartner
		{
			if($GLOBALS['fxpglobals']['persdat']['elter'])
				$GLOBALS['fxpglobals']['filter']['person_alias'][0]=$GLOBALS['fxpglobals']['persdat']['elter'];
			else
				$GLOBALS['fxpglobals']['filter']['person_alias'][0]=$GLOBALS['fxpglobals']['person'];
		}
	}

	// Set date limits
	if(!$err)
		setDateLimits();
}

// Check if user is already loged in
if(!$GLOBALS['fxpglobals']['user'])
{
	$GLOBALS['fxptdata']['lvalues']=array();

	// Login screen
	$err=false;
	setForm('login_3');

	// Delete old temporary and log files
	fxDeleteTMP();
	fxDeleteLOG();

	$GLOBALS['fxpglobals']['dbparam']['dbcount']='#';
	$GLOBALS['fxpglobals']['suser']=0;

	if($uid == 'logout')
	{
		$mht='Logout';
		$mct='<i>'.meldung(784).'</i><br /><br /><br /><br /><br /><br />';	// Your fx-project session has been ended. You have been successfully logged out of the system.


		$content .= '<div class="inlay" style="position:relative;width:640px;">'.$nl;
		$content .= '	<table border="0" cellpadding="3" cellspacing="10" width="100%">'.$nl;
		$content .= '		<tr>'.$nl;
		$content .= '			<td nowrap><div class="inlayhead" style="position:relative;text-align:center;">'.$mht.'</div></td>'.$nl;
		$content .= '		</tr>'.$nl;
		$content .= '		<tr>'.$nl;
		$content .= '			<td align="center"><br />'.$mct.'</td>'.$nl;
		$content .= '		</tr>'.$nl;
		$content .= '		<tr>'.$nl;
		$content .= '			<td align="center">';
	}

	// Set login fields if client 1 was just created
	if(isset($__c1ex) && ($__c1ex === 'client1'))
	{
		$maskenfeldwerte['name_mandant']='1';
		$maskenfeldwerte['name_benutzer']='admin';
	}
	// Set login fields according to cookie
	else
	{
		$maskenfeldwerte['name_mandant']=fxSession('t0','g','Name_Mandant');
		$maskenfeldwerte['name_benutzer']=fxSession('t0','g','Name_Benutzer');
	}
	if(($uid != 'Button_Login') && ($GLOBALS['fxpglobals']['client'] > 0))
	{
		if(!strlen($maskenfeldwerte['name_mandant']))
			$maskenfeldwerte['name_mandant']=$GLOBALS['fxpglobals']['client'];
		if(!strlen($maskenfeldwerte['name_benutzer']))
			$maskenfeldwerte['name_benutzer']=$GLOBALS['fxpglobals']['uname'];
	}

	// Never convert this mask fields into read-only
	$maske44_sf=true;

	$content .= maske(44);
	$content .= '				<br /><b style="font-size:8pt;">Powered by</b><br />'.($GLOBALS['_elinks']?'<a href="https://www.ferox.de" target="_blank" style="text-decoration:none;">':'').'<b style="font-size:12pt;">FeRox</b>'.($GLOBALS['_elinks']?'</a>':'').$nl;
	if($uid == 'logout')
	{
		$content .= '				<br /><br />'.$nl;
		$content .= '			</td>'.$nl;
		$content .= '		</tr>'.$nl;
		$content .= '	</table>'.$nl;
		$content .= '</div>'.$nl;
	}

	std_screen($prehtml, $content);

	// Language(s)
	$sla=fxGetAllLanguages(false,true);

	// Design
	tGetDesign();
	echo('<div id="ps_design" style="display:none;">'.$GLOBALS['ps_design'].'</div>');
	$fxdst=fxpGetDesigns();
	echo('<div id="tb_design" class="" style="display:none;">'.$fxdst.'</div>');

	$GLOBALS['fxpglobals']['dbparam']['status']=$status;
	return;
}

getPrgClass();
setPRole();

$in_kopf_v=false;

if(!$GLOBALS['fxpglobals']['client'] && isset($_POST['Mandanten_ID']) && $_POST['Mandanten_ID'])
	$GLOBALS['fxpglobals']['client']=(int)$_POST['Mandanten_ID'];
?>