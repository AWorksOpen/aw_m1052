/*******************************************************************************
*                                    AWorks
*                         ----------------------------
*                         innovating embedded platform
*
* Copyright (c) 2001-2019 Guangzhou ZHIYUAN Electronics Co., Ltd.
* All rights reserved.
*
* Contact information:
* web site:    https://www.zlg.cn
*******************************************************************************/

/**
 * \file
 * \brief Wi-Fi
 *
 * \internal
 * \par modification history
 * - 1.00 17-06-23  phd, first implementation
 * \endinternal
 */
#ifndef __AW_WIFI_TYPES_H
#define __AW_WIFI_TYPES_H
#ifdef __cplusplus
extern "C" {
#endif
#include "aw_bitops.h"
#define AW_WIFI_MAX_DEVICENAME_LEN          16
#define AW_WIFI_MAX_SSID_LEN                32
#define AW_WIFI_MAX_PASSPHRASE_LEN          64
#define AW_WIFI_ID_INVALID                  NULL
/**
 * Enumerated list of country codes
 */
#ifdef IL_BIGENDIAN
#define __MK_CNTRY(a, b, rev) \
(((unsigned char)(b)) + (((unsigned char)(a))<<8) + (((unsigned short)(rev))<<16))
#else /* ifdef IL_BIGENDIAN */
#define __MK_CNTRY(a, b, rev) \
(((unsigned char)(a)) + (((unsigned char)(b))<<8) + (((unsigned short)(rev))<<16))
#endif /* ifdef IL_BIGENDIAN */
typedef enum aw_wifi_country_code {
    
    /** \brief AF Afghanistan */
    AW_WIFI_COUNTRY_AFGHANISTAN                     = __MK_CNTRY('A', 'F', 0),             
    
    /** \brief AL Albania */
    AW_WIFI_COUNTRY_ALBANIA                         = __MK_CNTRY('A', 'L', 0),             
    
    /** \brief DZ Algeria */
    AW_WIFI_COUNTRY_ALGERIA                         = __MK_CNTRY('D', 'Z', 0),             
    
    /** \brief AS American_Samoa */
    AW_WIFI_COUNTRY_AMERICAN_SAMOA                  = __MK_CNTRY('A', 'S', 0),     

    /** \brief AO Angola */
    AW_WIFI_COUNTRY_ANGOLA                          = __MK_CNTRY('A', 'O', 0),             
    
    /** \brief AI Anguilla */
    AW_WIFI_COUNTRY_ANGUILLA                        = __MK_CNTRY('A', 'I', 0),             
    
    /** \brief AG Antigua_and_Barbuda */
    AW_WIFI_COUNTRY_ANTIGUA_AND_BARBUDA             = __MK_CNTRY('A', 'G', 0),             
    
    /** \brief AR Argentina */
    AW_WIFI_COUNTRY_ARGENTINA                       = __MK_CNTRY('A', 'R', 0),             
    
    /** \brief AM Armenia */
    AW_WIFI_COUNTRY_ARMENIA                         = __MK_CNTRY('A', 'M', 0),             
    
    /** \brief AW Aruba */
    AW_WIFI_COUNTRY_ARUBA                           = __MK_CNTRY('A', 'W', 0),             
    
    /** \brief AU Australia */
    AW_WIFI_COUNTRY_AUSTRALIA                       = __MK_CNTRY('A', 'U', 0),             
    
    /** \brief AT Austria */
    AW_WIFI_COUNTRY_AUSTRIA                         = __MK_CNTRY('A', 'T', 0),             
    
    /** \brief AZ Azerbaijan */
    AW_WIFI_COUNTRY_AZERBAIJAN                      = __MK_CNTRY('A', 'Z', 0),             
    
    /** \brief BS Bahamas */
    AW_WIFI_COUNTRY_BAHAMAS                         = __MK_CNTRY('B', 'S', 0),             
    
    /** \brief BH Bahrain */
    AW_WIFI_COUNTRY_BAHRAIN                         = __MK_CNTRY('B', 'H', 0),             
    
    /** \brief 0B Baker_Island */
    AW_WIFI_COUNTRY_BAKER_ISLAND                    = __MK_CNTRY('0', 'B', 0),             
    
    /** \brief BD Bangladesh */
    AW_WIFI_COUNTRY_BANGLADESH                      = __MK_CNTRY('B', 'D', 0),             
    
    /** \brief BB Barbados */
    AW_WIFI_COUNTRY_BARBADOS                        = __MK_CNTRY('B', 'B', 0),             
    
    /** \brief BY Belarus */
    AW_WIFI_COUNTRY_BELARUS                         = __MK_CNTRY('B', 'Y', 0),             
    
    /** \brief BE Belgium */
    AW_WIFI_COUNTRY_BELGIUM                         = __MK_CNTRY('B', 'E', 0),             
    
    /** \brief BZ Belize */
    AW_WIFI_COUNTRY_BELIZE                          = __MK_CNTRY('B', 'Z', 0),            
    
    /** \brief BJ Benin */
    AW_WIFI_COUNTRY_BENIN                           = __MK_CNTRY('B', 'J', 0),             
    
    /** \brief BM Bermuda */
    AW_WIFI_COUNTRY_BERMUDA                         = __MK_CNTRY('B', 'M', 0),             
    
    /** \brief BT Bhutan */
    AW_WIFI_COUNTRY_BHUTAN                          = __MK_CNTRY('B', 'T', 0),             
    
    /** \brief BO Bolivia */
    AW_WIFI_COUNTRY_BOLIVIA                         = __MK_CNTRY('B', 'O', 0),             
    
    /** \brief BA Bosnia_and_Herzegovina */
    AW_WIFI_COUNTRY_BOSNIA_AND_HERZEGOVINA          = __MK_CNTRY('B', 'A', 0),             
    
    /** \brief BW Botswana */
    AW_WIFI_COUNTRY_BOTSWANA                        = __MK_CNTRY('B', 'W', 0),             
    
    /** \brief BR Brazil */
    AW_WIFI_COUNTRY_BRAZIL                          = __MK_CNTRY('B', 'R', 0),             
    
    /** \brief IO British_Indian_Ocean_Territory */
    AW_WIFI_COUNTRY_BRITISH_INDIAN_OCEAN_TERRITOR   = __MK_CNTRY('I', 'O', 0),             
    
    /** \brief BN Brunei_Darussalam */
    AW_WIFI_COUNTRY_BRUNEI_DARUSSALAM               = __MK_CNTRY('B', 'N', 0),             
    
    /** \brief BG Bulgaria */
    AW_WIFI_COUNTRY_BULGARIA                        = __MK_CNTRY('B', 'G', 0),             
    
    /** \brief BF Burkina_Faso */
    AW_WIFI_COUNTRY_BURKINA_FASO                    = __MK_CNTRY('B', 'F', 0),             
    
    /** \brief BI Burundi */
    AW_WIFI_COUNTRY_BURUNDI                         = __MK_CNTRY('B', 'I', 0),             
    
    /** \brief KH Cambodia */
    AW_WIFI_COUNTRY_CAMBODIA                        = __MK_CNTRY('K', 'H', 0),             
    
    /** \brief CM Cameroon */
    AW_WIFI_COUNTRY_CAMEROON                        = __MK_CNTRY('C', 'M', 0),             
    
    /** \brief CA Canada */
    AW_WIFI_COUNTRY_CANADA                          = __MK_CNTRY('C', 'A', 0),             
    
    /** \brief CA Canada Revision 950 */
    AW_WIFI_COUNTRY_CANADA_REV950                   = __MK_CNTRY('C', 'A', 950),           
    
    /** \brief CV Cape_Verde */
    AW_WIFI_COUNTRY_CAPE_VERDE                      = __MK_CNTRY('C', 'V', 0),             
    
    /** \brief KY Cayman_Islands */
    AW_WIFI_COUNTRY_CAYMAN_ISLANDS                  = __MK_CNTRY('K', 'Y', 0),             
    
    /** \brief CF Central_African_Republic */
    AW_WIFI_COUNTRY_CENTRAL_AFRICAN_REPUBLIC        = __MK_CNTRY('C', 'F', 0),             
    
    /** \brief TD Chad */
    AW_WIFI_COUNTRY_CHAD                            = __MK_CNTRY('T', 'D', 0),             
    
    /** \brief CL Chile */
    AW_WIFI_COUNTRY_CHILE                           = __MK_CNTRY('C', 'L', 0),             
    
    /** \brief CN China */
    AW_WIFI_COUNTRY_CHINA                           = __MK_CNTRY('C', 'N', 0),             
    
    /** \brief CX Christmas_Island */
    AW_WIFI_COUNTRY_CHRISTMAS_ISLAND                = __MK_CNTRY('C', 'X', 0),             
    
    /** \brief CO Colombia */
    AW_WIFI_COUNTRY_COLOMBIA                        = __MK_CNTRY('C', 'O', 0),             
    
    /** \brief KM Comoros */
    AW_WIFI_COUNTRY_COMOROS                         = __MK_CNTRY('K', 'M', 0),             
    
    /** \brief CG Congo */
    AW_WIFI_COUNTRY_CONGO                           = __MK_CNTRY('C', 'G', 0),             
    
    /** \brief CD Congo,_The_Democratic_Republic_Of_The */
    AW_WIFI_COUNTRY_CONGO_THE_DEMOCRATIC_REPUBLIC_OF_THE = __MK_CNTRY('C', 'D', 0),             
                                                      
    /** \brief CR Costa_Rica */
    AW_WIFI_COUNTRY_COSTA_RICA                      = __MK_CNTRY('C', 'R', 0),             
    
    /** \brief CI Cote_D'ivoire */
    AW_WIFI_COUNTRY_COTE_DIVOIRE                    = __MK_CNTRY('C', 'I', 0),             
    
    /** \brief HR Croatia */
    AW_WIFI_COUNTRY_CROATIA                         = __MK_CNTRY('H', 'R', 0),             
    
    /** \brief CU Cuba */
    AW_WIFI_COUNTRY_CUBA                            = __MK_CNTRY('C', 'U', 0),             
    
    /** \brief CY Cyprus */
    AW_WIFI_COUNTRY_CYPRUS                          = __MK_CNTRY('C', 'Y', 0),             
    
    /** \brief CZ Czech_Republic */
    AW_WIFI_COUNTRY_CZECH_REPUBLIC                  = __MK_CNTRY('C', 'Z', 0),             
    
    /** \brief DK Denmark */
    AW_WIFI_COUNTRY_DENMARK                         = __MK_CNTRY('D', 'K', 0),             
    
    /** \brief DJ Djibouti */
    AW_WIFI_COUNTRY_DJIBOUTI                        = __MK_CNTRY('D', 'J', 0),             
    
    /** \brief DM Dominica */
    AW_WIFI_COUNTRY_DOMINICA                        = __MK_CNTRY('D', 'M', 0),             
    
    /** \brief DO Dominican_Republic */
    AW_WIFI_COUNTRY_DOMINICAN_REPUBLIC              = __MK_CNTRY('D', 'O', 0),             
    
    /** \brief AU G'Day mate! */
    AW_WIFI_COUNTRY_DOWN_UNDER                      = __MK_CNTRY('A', 'U', 0),             
    
    /** \brief EC Ecuador */
    AW_WIFI_COUNTRY_ECUADOR                         = __MK_CNTRY('E', 'C', 0),             
    
    /** \brief EG Egypt */
    AW_WIFI_COUNTRY_EGYPT                           = __MK_CNTRY('E', 'G', 0),             
    
    /** \brief SV El_Salvador */
    AW_WIFI_COUNTRY_EL_SALVADOR                     = __MK_CNTRY('S', 'V', 0),             
    
    /** \brief GQ Equatorial_Guinea */
    AW_WIFI_COUNTRY_EQUATORIAL_GUINEA               = __MK_CNTRY('G', 'Q', 0),             
    
    /** \brief ER Eritrea */
    AW_WIFI_COUNTRY_ERITREA                         = __MK_CNTRY('E', 'R', 0),             
    
    /** \brief EE Estonia */
    AW_WIFI_COUNTRY_ESTONIA                         = __MK_CNTRY('E', 'E', 0),             
    
    /** \brief ET Ethiopia */
    AW_WIFI_COUNTRY_ETHIOPIA                        = __MK_CNTRY('E', 'T', 0),             
    
    /** \brief FK Falkland_Islands_(Malvinas) */
    AW_WIFI_COUNTRY_FALKLAND_ISLANDS_MALVINAS       = __MK_CNTRY('F', 'K', 0),             
    
    /** \brief FO Faroe_Islands */
    AW_WIFI_COUNTRY_FAROE_ISLANDS                   = __MK_CNTRY('F', 'O', 0),             
    
    /** \brief FJ Fiji */
    AW_WIFI_COUNTRY_FIJI                            = __MK_CNTRY('F', 'J', 0),             
    
    /** \brief FI Finland */
    AW_WIFI_COUNTRY_FINLAND                         = __MK_CNTRY('F', 'I', 0),             
    
    /** \brief FR France */
    AW_WIFI_COUNTRY_FRANCE                          = __MK_CNTRY('F', 'R', 0),             
    
    /** \brief GF French_Guina */
    AW_WIFI_COUNTRY_FRENCH_GUINA                    = __MK_CNTRY('G', 'F', 0),             
    
    /** \brief PF French_Polynesia */
    AW_WIFI_COUNTRY_FRENCH_POLYNESIA                = __MK_CNTRY('P', 'F', 0),             
    
    /** \brief TF French_Southern_Territories */
    AW_WIFI_COUNTRY_FRENCH_SOUTHERN_TERRITORIES     = __MK_CNTRY('T', 'F', 0),             
    
    /** \brief GA Gabon */
    AW_WIFI_COUNTRY_GABON                           = __MK_CNTRY('G', 'A', 0),             
    
    /** \brief GM Gambia */
    AW_WIFI_COUNTRY_GAMBIA                          = __MK_CNTRY('G', 'M', 0),             
    
    /** \brief GE Georgia */
    AW_WIFI_COUNTRY_GEORGIA                         = __MK_CNTRY('G', 'E', 0),             
    
    /** \brief DE Germany */
    AW_WIFI_COUNTRY_GERMANY                         = __MK_CNTRY('D', 'E', 0),             
    
    /** \brief E0 European_Wide Revision 895 */
    AW_WIFI_COUNTRY_EUROPEAN_WIDE_REV895            = __MK_CNTRY('E', '0', 895),           
    
    /** \brief GH Ghana */
    AW_WIFI_COUNTRY_GHANA                           = __MK_CNTRY('G', 'H', 0),             
    
    /** \brief GI Gibraltar */
    AW_WIFI_COUNTRY_GIBRALTAR                       = __MK_CNTRY('G', 'I', 0),             
    
    /** \brief GR Greece */
    AW_WIFI_COUNTRY_GREECE                          = __MK_CNTRY('G', 'R', 0),             
    
    /** \brief GD Grenada */
    AW_WIFI_COUNTRY_GRENADA                         = __MK_CNTRY('G', 'D', 0),             
    
    /** \brief GP Guadeloupe */
    AW_WIFI_COUNTRY_GUADELOUPE                      = __MK_CNTRY('G', 'P', 0),             
    
    /** \brief GU Guam */
    AW_WIFI_COUNTRY_GUAM                            = __MK_CNTRY('G', 'U', 0),             
    
    /** \brief GT Guatemala */
    AW_WIFI_COUNTRY_GUATEMALA                       = __MK_CNTRY('G', 'T', 0),             
    
    /** \brief GG Guernsey */
    AW_WIFI_COUNTRY_GUERNSEY                        = __MK_CNTRY('G', 'G', 0),             
    
    /** \brief GN Guinea */
    AW_WIFI_COUNTRY_GUINEA                          = __MK_CNTRY('G', 'N', 0),             
    
    /** \brief GW Guinea-bissau */
    AW_WIFI_COUNTRY_GUINEA_BISSAU                   = __MK_CNTRY('G', 'W', 0),             
    
    /** \brief GY Guyana */
    AW_WIFI_COUNTRY_GUYANA                          = __MK_CNTRY('G', 'Y', 0),             
    
    /** \brief HT Haiti */
    AW_WIFI_COUNTRY_HAITI                           = __MK_CNTRY('H', 'T', 0),             
    
    /** \brief VA Holy_See_(Vatican_City_State) */
    AW_WIFI_COUNTRY_HOLY_SEE_VATICAN_CITY_STATE     = __MK_CNTRY('V', 'A', 0),             
    
    /** \brief HN Honduras */
    AW_WIFI_COUNTRY_HONDURAS                        = __MK_CNTRY('H', 'N', 0),             
    
    /** \brief HK Hong_Kong */
    AW_WIFI_COUNTRY_HONG_KONG                       = __MK_CNTRY('H', 'K', 0),             
    
    /** \brief HU Hungary */
    AW_WIFI_COUNTRY_HUNGARY                         = __MK_CNTRY('H', 'U', 0),             
    
    /** \brief IS Iceland */
    AW_WIFI_COUNTRY_ICELAND                         = __MK_CNTRY('I', 'S', 0),             
    
    /** \brief IN India */
    AW_WIFI_COUNTRY_INDIA                           = __MK_CNTRY('I', 'N', 0),             
    
    /** \brief ID Indonesia */
    AW_WIFI_COUNTRY_INDONESIA                       = __MK_CNTRY('I', 'D', 0),             
    
    /** \brief IR Iran,_Islamic_Republic_Of */
    AW_WIFI_COUNTRY_IRAN_ISLAMIC_REPUBLIC_OF        = __MK_CNTRY('I', 'R', 0),             
    
    /** \brief IQ Iraq */
    AW_WIFI_COUNTRY_IRAQ                            = __MK_CNTRY('I', 'Q', 0),             
    
    /** \brief IE Ireland */
    AW_WIFI_COUNTRY_IRELAND                         = __MK_CNTRY('I', 'E', 0),             
    
    /** \brief IL Israel */
    AW_WIFI_COUNTRY_ISRAEL                          = __MK_CNTRY('I', 'L', 0),             
    
    /** \brief IT Italy */
    AW_WIFI_COUNTRY_ITALY                           = __MK_CNTRY('I', 'T', 0),             
    
    /** \brief JM Jamaica */
    AW_WIFI_COUNTRY_JAMAICA                         = __MK_CNTRY('J', 'M', 0),             
    
    /** \brief JP Japan */
    AW_WIFI_COUNTRY_JAPAN                           = __MK_CNTRY('J', 'P', 0),             
    
    /** \brief JE Jersey */
    AW_WIFI_COUNTRY_JERSEY                          = __MK_CNTRY('J', 'E', 0),             
    
    /** \brief JO Jordan */
    AW_WIFI_COUNTRY_JORDAN                          = __MK_CNTRY('J', 'O', 0),             
    
    /** \brief KZ Kazakhstan */
    AW_WIFI_COUNTRY_KAZAKHSTAN                      = __MK_CNTRY('K', 'Z', 0),             
    
    /** \brief KE Kenya */
    AW_WIFI_COUNTRY_KENYA                           = __MK_CNTRY('K', 'E', 0),             
    
    /** \brief KI Kiribati */
    AW_WIFI_COUNTRY_KIRIBATI                        = __MK_CNTRY('K', 'I', 0),             
    
    /** \brief KR Korea,_Republic_Of */
    AW_WIFI_COUNTRY_KOREA_REPUBLIC_OF               = __MK_CNTRY('K', 'R', 1),             
    
    /** \brief 0A Kosovo */
    AW_WIFI_COUNTRY_KOSOVO                          = __MK_CNTRY('0', 'A', 0),             
    
    /** \brief KW Kuwait */
    AW_WIFI_COUNTRY_KUWAIT                          = __MK_CNTRY('K', 'W', 0),             
    
    /** \brief KG Kyrgyzstan */
    AW_WIFI_COUNTRY_KYRGYZSTAN                      = __MK_CNTRY('K', 'G', 0),             
    
    /** \brief LA Lao_People's_Democratic_Repubic */
    AW_WIFI_COUNTRY_LAO_PEOPLES_DEMOCRATIC_REPUBIC  = __MK_CNTRY('L', 'A', 0),             
    
    /** \brief LV Latvia */
    AW_WIFI_COUNTRY_LATVIA                          = __MK_CNTRY('L', 'V', 0),             
    
    /** \brief LB Lebanon */
    AW_WIFI_COUNTRY_LEBANON                         = __MK_CNTRY('L', 'B', 0),             
    
    /** \brief LS Lesotho */
    AW_WIFI_COUNTRY_LESOTHO                         = __MK_CNTRY('L', 'S', 0),             
    
    /** \brief LR Liberia */
    AW_WIFI_COUNTRY_LIBERIA                         = __MK_CNTRY('L', 'R', 0),             
    
    /** \brief LY Libyan_Arab_Jamahiriya */
    AW_WIFI_COUNTRY_LIBYAN_ARAB_JAMAHIRIYA          = __MK_CNTRY('L', 'Y', 0),            
    
    /** \brief LI Liechtenstein */
    AW_WIFI_COUNTRY_LIECHTENSTEIN                   = __MK_CNTRY('L', 'I', 0),             
    
    /** \brief LT Lithuania */
    AW_WIFI_COUNTRY_LITHUANIA                       = __MK_CNTRY('L', 'T', 0),             
    
    /** \brief LU Luxembourg */
    AW_WIFI_COUNTRY_LUXEMBOURG                      = __MK_CNTRY('L', 'U', 0),             
    
    /** \brief MO Macao */
    AW_WIFI_COUNTRY_MACAO                           = __MK_CNTRY('M', 'O', 0),  

    /** \brief MK Macedonia,_Former_Yugoslav_Republic_Of */
    AW_WIFI_COUNTRY_MACEDONIA_FORMER_YUGOSLAV_REPUBLIC_OF = __MK_CNTRY('M', 'K', 0),             
                                                      
    /** \brief MG Madagascar */
    AW_WIFI_COUNTRY_MADAGASCAR                      = __MK_CNTRY('M', 'G', 0),             
    
    /** \brief MW Malawi */
    AW_WIFI_COUNTRY_MALAWI                          = __MK_CNTRY('M', 'W', 0),             
    
    /** \brief MY Malaysia */
    AW_WIFI_COUNTRY_MALAYSIA                        = __MK_CNTRY('M', 'Y', 0),             
    
    /** \brief MV Maldives */
    AW_WIFI_COUNTRY_MALDIVES                        = __MK_CNTRY('M', 'V', 0),             
    
    /** \brief ML Mali */
    AW_WIFI_COUNTRY_MALI                            = __MK_CNTRY('M', 'L', 0),             
    
    /** \brief MT Malta */
    AW_WIFI_COUNTRY_MALTA                           = __MK_CNTRY('M', 'T', 0),             
    
    /** \brief IM Man,_Isle_Of */
    AW_WIFI_COUNTRY_MAN_ISLE_OF                     = __MK_CNTRY('I', 'M', 0),             
    
    /** \brief MQ Martinique */
    AW_WIFI_COUNTRY_MARTINIQUE                      = __MK_CNTRY('M', 'Q', 0),             
    
    /** \brief MR Mauritania */
    AW_WIFI_COUNTRY_MAURITANIA                      = __MK_CNTRY('M', 'R', 0),             
    
    /** \brief MU Mauritius */
    AW_WIFI_COUNTRY_MAURITIUS                       = __MK_CNTRY('M', 'U', 0),             
    
    /** \brief YT Mayotte */
    AW_WIFI_COUNTRY_MAYOTTE                         = __MK_CNTRY('Y', 'T', 0),             
    
    /** \brief MX Mexico */
    AW_WIFI_COUNTRY_MEXICO                          = __MK_CNTRY('M', 'X', 0),             
    
    /** \brief FM Micronesia,_Federated_States_Of */
    AW_WIFI_COUNTRY_MICRONESIA_FEDERATED_STATES_OF  = __MK_CNTRY('F', 'M', 0),             
    
    /** \brief MD Moldova,_Republic_Of */
    AW_WIFI_COUNTRY_MOLDOVA_REPUBLIC_OF             = __MK_CNTRY('M', 'D', 0),             
    
    /** \brief MC Monaco */
    AW_WIFI_COUNTRY_MONACO                          = __MK_CNTRY('M', 'C', 0),             
    
    /** \brief MN Mongolia */
    AW_WIFI_COUNTRY_MONGOLIA                        = __MK_CNTRY('M', 'N', 0),             
    
    /** \brief ME Montenegro */
    AW_WIFI_COUNTRY_MONTENEGRO                      = __MK_CNTRY('M', 'E', 0),             
    
    /** \brief MS Montserrat */
    AW_WIFI_COUNTRY_MONTSERRAT                      = __MK_CNTRY('M', 'S', 0),             
    
    /** \brief MA Morocco */
    AW_WIFI_COUNTRY_MOROCCO                         = __MK_CNTRY('M', 'A', 0),             
    
    /** \brief MZ Mozambique */
    AW_WIFI_COUNTRY_MOZAMBIQUE                      = __MK_CNTRY('M', 'Z', 0),             
    
    /** \brief MM Myanmar */
    AW_WIFI_COUNTRY_MYANMAR                         = __MK_CNTRY('M', 'M', 0),             
    
    /** \brief NA Namibia */
    AW_WIFI_COUNTRY_NAMIBIA                         = __MK_CNTRY('N', 'A', 0),             
    
    /** \brief NR Nauru */
    AW_WIFI_COUNTRY_NAURU                           = __MK_CNTRY('N', 'R', 0),             
    
    /** \brief NP Nepal */
    AW_WIFI_COUNTRY_NEPAL                           = __MK_CNTRY('N', 'P', 0),             
    
    /** \brief NL Netherlands */
    AW_WIFI_COUNTRY_NETHERLANDS                     = __MK_CNTRY('N', 'L', 0),             
    
    /** \brief AN Netherlands_Antilles */
    AW_WIFI_COUNTRY_NETHERLANDS_ANTILLES            = __MK_CNTRY('A', 'N', 0),             
    
    /** \brief NC New_Caledonia */
    AW_WIFI_COUNTRY_NEW_CALEDONIA                   = __MK_CNTRY('N', 'C', 0),             
    
    /** \brief NZ New_Zealand */
    AW_WIFI_COUNTRY_NEW_ZEALAND                     = __MK_CNTRY('N', 'Z', 0),             
    
    /** \brief NI Nicaragua */
    AW_WIFI_COUNTRY_NICARAGUA                       = __MK_CNTRY('N', 'I', 0),             
    
    /** \brief NE Niger */
    AW_WIFI_COUNTRY_NIGER                           = __MK_CNTRY('N', 'E', 0),             
    
    /** \brief NG Nigeria */
    AW_WIFI_COUNTRY_NIGERIA                         = __MK_CNTRY('N', 'G', 0),             
    
    /** \brief NF Norfolk_Island */
    AW_WIFI_COUNTRY_NORFOLK_ISLAND                  = __MK_CNTRY('N', 'F', 0),             
    
    /** \brief MP Northern_Mariana_Islands */
    AW_WIFI_COUNTRY_NORTHERN_MARIANA_ISLANDS        = __MK_CNTRY('M', 'P', 0),             
    
    /** \brief NO Norway */
    AW_WIFI_COUNTRY_NORWAY                          = __MK_CNTRY('N', 'O', 0),             
    
    /** \brief OM Oman */
    AW_WIFI_COUNTRY_OMAN                            = __MK_CNTRY('O', 'M', 0),             
    
    /** \brief PK Pakistan */
    AW_WIFI_COUNTRY_PAKISTAN                        = __MK_CNTRY('P', 'K', 0),             
    
    /** \brief PW Palau */
    AW_WIFI_COUNTRY_PALAU                           = __MK_CNTRY('P', 'W', 0),             
    
    /** \brief PA Panama */
    AW_WIFI_COUNTRY_PANAMA                          = __MK_CNTRY('P', 'A', 0),             
    
    /** \brief PG Papua_New_Guinea */
    AW_WIFI_COUNTRY_PAPUA_NEW_GUINEA                = __MK_CNTRY('P', 'G', 0),             
    
    /** \brief PY Paraguay */
    AW_WIFI_COUNTRY_PARAGUAY                        = __MK_CNTRY('P', 'Y', 0),             
    
    /** \brief PE Peru */
    AW_WIFI_COUNTRY_PERU                            = __MK_CNTRY('P', 'E', 0),             
    
    /** \brief PH Philippines */
    AW_WIFI_COUNTRY_PHILIPPINES                     = __MK_CNTRY('P', 'H', 0),             
    
    /** \brief PL Poland */
    AW_WIFI_COUNTRY_POLAND                          = __MK_CNTRY('P', 'L', 0),             
    
    /** \brief PT Portugal */
    AW_WIFI_COUNTRY_PORTUGAL                        = __MK_CNTRY('P', 'T', 0),             
    
    /** \brief PR Pueto_Rico */
    AW_WIFI_COUNTRY_PUETO_RICO                      = __MK_CNTRY('P', 'R', 0),             
    
    /** \brief QA Qatar */
    AW_WIFI_COUNTRY_QATAR                           = __MK_CNTRY('Q', 'A', 0),             
    
    /** \brief RE Reunion */
    AW_WIFI_COUNTRY_REUNION                         = __MK_CNTRY('R', 'E', 0),             
    
    /** \brief RO Romania */
    AW_WIFI_COUNTRY_ROMANIA                         = __MK_CNTRY('R', 'O', 0),             
    
    /** \brief RU Russian_Federation */
    AW_WIFI_COUNTRY_RUSSIAN_FEDERATION              = __MK_CNTRY('R', 'U', 0),             
    
    /** \brief RW Rwanda */
    AW_WIFI_COUNTRY_RWANDA                          = __MK_CNTRY('R', 'W', 0),             
    
    /** \brief KN Saint_Kitts_and_Nevis */
    AW_WIFI_COUNTRY_SAINT_KITTS_AND_NEVIS           = __MK_CNTRY('K', 'N', 0),             
    
    /** \brief LC Saint_Lucia */
    AW_WIFI_COUNTRY_SAINT_LUCIA                     = __MK_CNTRY('L', 'C', 0),             
    
    /** \brief PM Saint_Pierre_and_Miquelon  */
    AW_WIFI_COUNTRY_SAINT_PIERRE_AND_MIQUELON       = __MK_CNTRY('P', 'M', 0),             
    
    /** \brief VC Saint_Vincent_and_The_Grenadines */
    AW_WIFI_COUNTRY_SAINT_VINCENT_AND_THE_GRENADINES = __MK_CNTRY('V', 'C', 0),             
                                                    
    /** \brief WS Samoa */
    AW_WIFI_COUNTRY_SAMOA                           = __MK_CNTRY('W', 'S', 0),             
    
    /** \brief MF Sanit_Martin_/_Sint_Marteen */
    AW_WIFI_COUNTRY_SANIT_MARTIN_SINT_MARTEEN       = __MK_CNTRY('M', 'F', 0),             
    
    /** \brief ST Sao_Tome_and_Principe */
    AW_WIFI_COUNTRY_SAO_TOME_AND_PRINCIPE           = __MK_CNTRY('S', 'T', 0),             
    
    /** \brief SA Saudi_Arabia */
    AW_WIFI_COUNTRY_SAUDI_ARABIA                    = __MK_CNTRY('S', 'A', 0),             
    
    /** \brief SN Senegal */
    AW_WIFI_COUNTRY_SENEGAL                         = __MK_CNTRY('S', 'N', 0),             
    
    /** \brief RS Serbia */
    AW_WIFI_COUNTRY_SERBIA                          = __MK_CNTRY('R', 'S', 0),             
    
    /** \brief SC Seychelles */
    AW_WIFI_COUNTRY_SEYCHELLES                      = __MK_CNTRY('S', 'C', 0),             
    
    /** \brief SL Sierra_Leone */
    AW_WIFI_COUNTRY_SIERRA_LEONE                    = __MK_CNTRY('S', 'L', 0),             
    
    /** \brief SG Singapore */
    AW_WIFI_COUNTRY_SINGAPORE                       = __MK_CNTRY('S', 'G', 0),             
    
    /** \brief SK Slovakia */
    AW_WIFI_COUNTRY_SLOVAKIA                        = __MK_CNTRY('S', 'K', 0),             
    
    /** \brief SI Slovenia */
    AW_WIFI_COUNTRY_SLOVENIA                        = __MK_CNTRY('S', 'I', 0),             
    
    /** \brief SB Solomon_Islands */
    AW_WIFI_COUNTRY_SOLOMON_ISLANDS                 = __MK_CNTRY('S', 'B', 0),             
    
    /** \brief SO Somalia */
    AW_WIFI_COUNTRY_SOMALIA                         = __MK_CNTRY('S', 'O', 0),             
    
    /** \brief ZA South_Africa */
    AW_WIFI_COUNTRY_SOUTH_AFRICA                    = __MK_CNTRY('Z', 'A', 0),             
    
    /** \brief ES Spain */
    AW_WIFI_COUNTRY_SPAIN                           = __MK_CNTRY('E', 'S', 0),             
    
    /** \brief LK Sri_Lanka */
    AW_WIFI_COUNTRY_SRI_LANKA                       = __MK_CNTRY('L', 'K', 0),             
    
    /** \brief SR Suriname */
    AW_WIFI_COUNTRY_SURINAME                        = __MK_CNTRY('S', 'R', 0),             
    
    /** \brief SZ Swaziland */
    AW_WIFI_COUNTRY_SWAZILAND                       = __MK_CNTRY('S', 'Z', 0),             
    
    /** \brief SE Sweden */
    AW_WIFI_COUNTRY_SWEDEN                          = __MK_CNTRY('S', 'E', 0),             
    
    /** \brief CH Switzerland */
    AW_WIFI_COUNTRY_SWITZERLAND                     = __MK_CNTRY('C', 'H', 0),             
    
    /** \brief SY Syrian_Arab_Republic */
    AW_WIFI_COUNTRY_SYRIAN_ARAB_REPUBLIC            = __MK_CNTRY('S', 'Y', 0),             
    
    /** \brief TW Taiwan,_Province_Of_China */
    AW_WIFI_COUNTRY_TAIWAN_PROVINCE_OF_CHINA        = __MK_CNTRY('T', 'W', 0),             
    
    /** \brief TJ Tajikistan */
    AW_WIFI_COUNTRY_TAJIKISTAN                      = __MK_CNTRY('T', 'J', 0),             
    
    /** \brief TZ Tanzania,_United_Republic_Of */
    AW_WIFI_COUNTRY_TANZANIA_UNITED_REPUBLIC_OF     = __MK_CNTRY('T', 'Z', 0),             
    
    /** \brief TH Thailand */
    AW_WIFI_COUNTRY_THAILAND                        = __MK_CNTRY('T', 'H', 0),             
    
    /** \brief TG Togo */
    AW_WIFI_COUNTRY_TOGO                            = __MK_CNTRY('T', 'G', 0),             
    
    /** \brief TO Tonga */
    AW_WIFI_COUNTRY_TONGA                           = __MK_CNTRY('T', 'O', 0),             
    
    /** \brief TT Trinidad_and_Tobago */
    AW_WIFI_COUNTRY_TRINIDAD_AND_TOBAGO             = __MK_CNTRY('T', 'T', 0),             
    
    /** \brief TN Tunisia */
    AW_WIFI_COUNTRY_TUNISIA                         = __MK_CNTRY('T', 'N', 0),             
    
    /** \brief TR Turkey */
    AW_WIFI_COUNTRY_TURKEY                          = __MK_CNTRY('T', 'R', 0),             
    
    /** \brief TM Turkmenistan */
    AW_WIFI_COUNTRY_TURKMENISTAN                    = __MK_CNTRY('T', 'M', 0),             
    
    /** \brief TC Turks_and_Caicos_Islands */
    AW_WIFI_COUNTRY_TURKS_AND_CAICOS_ISLANDS        = __MK_CNTRY('T', 'C', 0),             
    
    /** \brief TV Tuvalu */
    AW_WIFI_COUNTRY_TUVALU                          = __MK_CNTRY('T', 'V', 0),             
    
    /** \brief UG Uganda */
    AW_WIFI_COUNTRY_UGANDA                          = __MK_CNTRY('U', 'G', 0),             
    
    /** \brief UA Ukraine */
    AW_WIFI_COUNTRY_UKRAINE                         = __MK_CNTRY('U', 'A', 0),             
    
    /** \brief AE United_Arab_Emirates */
    AW_WIFI_COUNTRY_UNITED_ARAB_EMIRATES            = __MK_CNTRY('A', 'E', 0),             
    
    /** \brief GB United_Kingdom */
    AW_WIFI_COUNTRY_UNITED_KINGDOM                  = __MK_CNTRY('G', 'B', 0),             
    
    /** \brief US United_States */
    AW_WIFI_COUNTRY_UNITED_STATES                   = __MK_CNTRY('U', 'S', 0),             
    
    /** \brief US United_States Revision 4 */
    AW_WIFI_COUNTRY_UNITED_STATES_REV4              = __MK_CNTRY('U', 'S', 4),             
    
    /** \brief Q1 United_States Revision 931 */
    AW_WIFI_COUNTRY_UNITED_STATES_REV931            = __MK_CNTRY('Q', '1', 931),           
    
    /** \brief Q2 United_States_(No_DFS) */
    AW_WIFI_COUNTRY_UNITED_STATES_NO_DFS            = __MK_CNTRY('Q', '2', 0),             
    
    /** \brief UM United_States_Minor_Outlying_Islands */
    AW_WIFI_COUNTRY_UNITED_STATES_MINOR_OUTLYING_ISLANDS = __MK_CNTRY('U', 'M', 0),             
    
    /** \brief UY Uruguay */
    AW_WIFI_COUNTRY_URUGUAY                         = __MK_CNTRY('U', 'Y', 0),             
    
    /** \brief UZ Uzbekistan */
    AW_WIFI_COUNTRY_UZBEKISTAN                      = __MK_CNTRY('U', 'Z', 0),             
    
    /** \brief VU Vanuatu */
    AW_WIFI_COUNTRY_VANUATU                         = __MK_CNTRY('V', 'U', 0),             
    
    /** \brief VE Venezuela */
    AW_WIFI_COUNTRY_VENEZUELA                       = __MK_CNTRY('V', 'E', 0),             
    
    /** \brief VN Viet_Nam */
    AW_WIFI_COUNTRY_VIET_NAM                        = __MK_CNTRY('V', 'N', 0),             
    
    /** \brief VG Virgin_Islands,_British */
    AW_WIFI_COUNTRY_VIRGIN_ISLANDS_BRITISH          = __MK_CNTRY('V', 'G', 0),             
    
    /** \brief VI Virgin_Islands,_U.S. */
    AW_WIFI_COUNTRY_VIRGIN_ISLANDS_US               = __MK_CNTRY('V', 'I', 0),             
    
    /** \brief WF Wallis_and_Futuna */
    AW_WIFI_COUNTRY_WALLIS_AND_FUTUNA               = __MK_CNTRY('W', 'F', 0),             
    
    /** \brief 0C West_Bank */
    AW_WIFI_COUNTRY_WEST_BANK                       = __MK_CNTRY('0', 'C', 0),             
    
    /** \brief EH Western_Sahara */
    AW_WIFI_COUNTRY_WESTERN_SAHARA                  = __MK_CNTRY('E', 'H', 0),             
    
    /** \brief Worldwide Locale Revision 983 */
    AW_WIFI_COUNTRY_WORLD_WIDE_XV_REV983            = __MK_CNTRY('X', 'V', 983),           
    
    /** \brief Worldwide Locale (passive Ch12-14) */
    AW_WIFI_COUNTRY_WORLD_WIDE_XX                   = __MK_CNTRY('X', 'X', 0),             
    
    /** \brief Worldwide Locale (passive Ch12-14) Revision 17 */
    AW_WIFI_COUNTRY_WORLD_WIDE_XX_REV17             = __MK_CNTRY('X', 'X', 17),            
    
    /** \brief YE Yemen */
    AW_WIFI_COUNTRY_YEMEN                           = __MK_CNTRY('Y', 'E', 0),             
    
    /** \brief ZM Zambia */
    AW_WIFI_COUNTRY_ZAMBIA                          = __MK_CNTRY('Z', 'M', 0),             
    
    /** \brief ZW Zimbabwe */
    AW_WIFI_COUNTRY_ZIMBABWE                        = __MK_CNTRY('Z', 'W', 0),             
} aw_wifi_country_code_t;
#undef __MK_CNTRY
/**
 * \brief Enumeration of work modes
 */
typedef enum aw_wifi_opmode {
    AW_WIFI_MODE_STOP           = 0,
    AW_WIFI_MODE_STA_ONLY       = AW_BIT(0),
    AW_WIFI_MODE_AP_ONLY        = AW_BIT(1),
    AW_WIFI_MODE_APSTA          = (AW_WIFI_MODE_STA_ONLY | AW_WIFI_MODE_AP_ONLY),
    AW_WIFI_MODE_FORCE_16_BIT   = 0x7fff
} aw_wifi_opmode_t;
/**
 * \brief Enumeration of run status
 */
typedef enum aw_wifi_state {
    AW_WIFI_STATE_UNAVALIABLE   = 0,
    AW_WIFI_STATE_EXIST         = AW_BIT(0),  /**< \brief exist or not exist */
    AW_WIFI_STATE_POWER         = AW_BIT(1),  /**< \brief power on or off */
    AW_WIFI_STATE_UP            = AW_BIT(2),  /**< \brief device core is up or not */

    AW_WIFI_STATE_AP_ON         = AW_BIT(3),  /**< \brief soft ap is on */
    AW_WIFI_STATE_STA_JOINED    = AW_BIT(4),  /**< \brief sta is joined */
    AW_WIFI_STATE_FUNCTIONAL    = (AW_WIFI_STATE_EXIST | AW_WIFI_STATE_POWER | AW_WIFI_STATE_UP),
    AW_WIFI_STATE_FORCE_16_BIT  = 0x7fff
} aw_wifi_state_t;
/**
 * \brief Enumeration of network types
 */
typedef enum aw_wifi_bss_type {
    
    /** \brief Denotes infrastructure network */
    AW_WIFI_BSS_TYPE_INFRASTRUCTURE =  0,       
    
    /** \brief Denotes an 802.11 ad-hoc IBSS network           */
    AW_WIFI_BSS_TYPE_ADHOC          =  1,       
    
    /** \brief Denotes either infrastructure or ad-hoc network */
    AW_WIFI_BSS_TYPE_ANY            =  2,       
    
    /** 
     *\brief May be returned by scan function if BSS type is unknown.
     *       Do not pass this to the Join function           
     */
    AW_WIFI_BSS_TYPE_UNKNOWN        = -1        
} aw_wifi_bss_type_t;
/**
 * \brief Enumeration of 802.11 radio bands
 */
typedef enum aw_802_11_band {
    AW_WIFI_802_11_BAND_5GHZ   = 0,     /**< \brief Denotes 5GHz radio band   */
    AW_WIFI_802_11_BAND_2_4GHZ = 1      /**< \brief Denotes 2.4GHz radio band */
} aw_wifi_802_11_band_t;
/**
 * \brief Service Set Identifier (i.e. Name of Access Point)
 */
typedef struct aw_wifi_ssid {
    uint8_t length;                         /**< \brief SSID length */
    uint8_t pad0[3];                        /**< \brief Padding */
    uint8_t value[AW_WIFI_MAX_SSID_LEN + 1];    /**< \brief SSID name (AP name)  */
} aw_wifi_ssid_t;
/**
 * \brief Pre-Share Key or PMK (i.e. Password of Access Point)
 * If the length is 64, the password is regarded as PMK.
 */
typedef struct aw_wifi_key {
    uint8_t length;                             /**< \brief key length */
    uint8_t pad0[3];                            /**< \brief Padding */
    uint8_t value[AW_WIFI_MAX_PASSPHRASE_LEN + 1];  /**< \brief key content  */
} aw_wifi_key_t;
/**
 * \brief Enumeration of Wi-Fi security modes
 */
#ifdef __WEP_ENABLED
#undef __WEP_ENABLED
#endif
#ifdef __TKIP_ENABLED
#undef __TKIP_ENABLED
#endif
#ifdef __AES_ENABLED
#undef __AES_ENABLED
#endif
#ifdef __SHARED_ENABLED
#undef __SHARED_ENABLED
#endif
#ifdef __WPA_SECURITY
#undef __WPA_SECURITY
#endif
#ifdef __WPA2_SECURITY
#undef __WPA2_SECURITY
#endif
#ifdef __ENT_ENABLED
#undef __ENT_ENABLED
#endif
#ifdef __WPS_ENABLED
#undef __WPS_ENABLED
#endif
#ifdef __IBSS_ENABLED
#undef __IBSS_ENABLED
#endif
#define __WEP_ENABLED                 0x0001
#define __TKIP_ENABLED                0x0002
#define __AES_ENABLED                 0x0004
#define __SHARED_ENABLED          0x00008000
#define __WPA_SECURITY            0x00200000
#define __WPA2_SECURITY           0x00400000
#define __ENT_ENABLED             0x02000000
#define __WPS_ENABLED             0x10000000
#define __IBSS_ENABLED            0x20000000
typedef enum aw_wifi_security {
    
    /** \brief Open security                                         */
    AW_WIFI_SECURITY_OPEN           = 0,                                                                   
    
    /** \brief WEP PSK Security with open authentication             */
    AW_WIFI_SECURITY_WEP_PSK        = __WEP_ENABLED,                                                       
    
    /** \brief WEP PSK Security with shared authentication           */
    AW_WIFI_SECURITY_WEP_SHARED     = (__WEP_ENABLED   | __SHARED_ENABLED),                                
    
    /** \brief WPA PSK Security with TKIP                            */
    AW_WIFI_SECURITY_WPA_TKIP_PSK   = (__WPA_SECURITY  | __TKIP_ENABLED),                                  
    
    /** \brief WPA PSK Security with AES                             */
    AW_WIFI_SECURITY_WPA_AES_PSK    = (__WPA_SECURITY  | __AES_ENABLED),                                   
    
    /** \brief WPA PSK Security with AES & TKIP                      */
    AW_WIFI_SECURITY_WPA_MIXED_PSK  = (__WPA_SECURITY  | __AES_ENABLED | __TKIP_ENABLED),                  
    
    /** \brief WPA2 PSK Security with AES                            */
    AW_WIFI_SECURITY_WPA2_AES_PSK   = (__WPA2_SECURITY | __AES_ENABLED),                                   
    
    /** \brief WPA2 PSK Security with TKIP                           */
    AW_WIFI_SECURITY_WPA2_TKIP_PSK  = (__WPA2_SECURITY | __TKIP_ENABLED),                                  
    
    /** \brief WPA2 PSK Security with AES & TKIP                     */
    AW_WIFI_SECURITY_WPA2_MIXED_PSK = (__WPA2_SECURITY | __AES_ENABLED | __TKIP_ENABLED),                  
    
    /** \brief WPA Enterprise Security with TKIP                     */
    AW_WIFI_SECURITY_WPA_TKIP_ENT   = (__ENT_ENABLED | __WPA_SECURITY  | __TKIP_ENABLED),                  
    
    /** \brief WPA Enterprise Security with AES                      */
    AW_WIFI_SECURITY_WPA_AES_ENT    = (__ENT_ENABLED | __WPA_SECURITY  | __AES_ENABLED),                   
    
    /** \brief WPA Enterprise Security with AES & TKIP               */
    AW_WIFI_SECURITY_WPA_MIXED_ENT  = (__ENT_ENABLED | __WPA_SECURITY  | __AES_ENABLED | __TKIP_ENABLED),  
    
    /** \brief WPA2 Enterprise Security with TKIP                    */
    AW_WIFI_SECURITY_WPA2_TKIP_ENT  = (__ENT_ENABLED | __WPA2_SECURITY | __TKIP_ENABLED),                  
    
    /** \brief WPA2 Enterprise Security with AES                     */
    AW_WIFI_SECURITY_WPA2_AES_ENT   = (__ENT_ENABLED | __WPA2_SECURITY | __AES_ENABLED),                   
    
    /** \brief WPA2 Enterprise Security with AES & TKIP              */
    AW_WIFI_SECURITY_WPA2_MIXED_ENT = (__ENT_ENABLED | __WPA2_SECURITY | __AES_ENABLED | __TKIP_ENABLED),  
    
    /** \brief Open security on IBSS ad-hoc network                  */
    AW_WIFI_SECURITY_IBSS_OPEN      = (__IBSS_ENABLED),                                                    
    
    /** \brief WPS with open security                                */
    AW_WIFI_SECURITY_WPS_OPEN       = (__WPS_ENABLED),                                                     
    
    /** \brief WPS with AES security                                 */
    AW_WIFI_SECURITY_WPS_SECURE     = (__WPS_ENABLED | __AES_ENABLED),                                     
    
    /**
     *\brief May be returned by scan function if security is unknown. 
     * Do not pass this to the join function!
     */     
    AW_WIFI_SECURITY_UNKNOWN        = -1,                                                                  
    
    /** \brief Exists only to force wiced_security_t type to 32 bits */
    AW_WIFI_SECURITY_FORCE_32_BIT   = 0x7fffffff                                                           
} aw_wifi_security_t;
#ifdef __WEP_ENABLED
#undef __WEP_ENABLED
#endif
#ifdef __TKIP_ENABLED
#undef __TKIP_ENABLED
#endif
#ifdef __AES_ENABLED
#undef __AES_ENABLED
#endif
#ifdef __SHARED_ENABLED
#undef __SHARED_ENABLED
#endif
#ifdef __WPA_SECURITY
#undef __WPA_SECURITY
#endif
#ifdef __WPA2_SECURITY
#undef __WPA2_SECURITY
#endif
#ifdef __ENT_ENABLED
#undef __ENT_ENABLED
#endif
#ifdef __WPS_ENABLED
#undef __WPS_ENABLED
#endif
#ifdef __IBSS_ENABLED
#undef __IBSS_ENABLED
#endif
/**
 * \brief Wi-Fi Device Control Handle
 */
typedef void * aw_wifi_id_t;
/**
 * \brief Enumeration of Wi-Fi interfaces
 */
typedef enum aw_wifi_interface {
    AW_WIFI_STA_INTERFACE = 0,  /**< \brief STA or Client Interface        */
    AW_WIFI_AP_INTERFACE  = 1,  /**< \brief AP  Interface                  */
    AW_WIFI_INTERFACE_MAX,      /**< \brief used for counting interfaces   */
} aw_wifi_interface_t;
/**
 * \brief Wi-Fi Media Access Control address
 */
typedef struct {
    uint8_t octet[6];
} aw_wifi_mac_t;
/**
 * \brief Wi-Fi configuration for initialization in STA mode.
 */
typedef struct aw_wifi_sta_config {
    
    /** \brief The SSID of the target AP. */
    aw_wifi_ssid_t     ssid;             
    
    /** \brief The password of the target AP. */
    aw_wifi_key_t      key;              
    
    /** 
     *\brief The BSSID is present if it is set to 1.Otherwise,it is set to 0.
     */
    uint8_t            bssid_present;    
    
    /** \brief The MAC address of the target AP. */
    aw_wifi_mac_t      bssid;            
    
    /**
     *\brief The security is present if it is set to 1. 
     * Otherwise, it is set to 0. 
     */
    uint8_t            security_present; 
    
    /** \brief The security type. */
    aw_wifi_security_t security;         
    
    /** \brief The STA is assocated to an AP. */
    uint8_t            is_joined;        
} aw_wifi_sta_config_t;

/**
 * \brief Wi-Fi AP run conf & state.
 */
#define AW_WIFI_AP_HIDDEN AW_BIT(0)

/**
 * \brief Wi-Fi configuration for initialization in AP mode.
 */
typedef struct aw_wifi_ap_config {
    
    aw_wifi_ssid_t     ssid;              /**< \brief The SSID of the AP. */
    aw_wifi_key_t      key;               /**< \brief The password of the AP. */
    aw_wifi_security_t security;          /**< \brief The security type. */
    uint8_t            channel;           /**< \brief The channel. */
    uint8_t            flags;             /**< \brief AP run conf & state. */
} aw_wifi_ap_config_t;
/**
 * \brief Wi-Fi configuration for initialization.
 */
typedef struct aw_wifi_config {
    aw_wifi_opmode_t     opmode;  /**< \brief Current operation mode.*/
    aw_wifi_sta_config_t sta;     /**< \brief The configurations for the STA. */
    aw_wifi_ap_config_t  ap;      /**< \brief The configurations for the AP. */
} aw_wifi_config_t;
/**
 * \brief config status.
 */
typedef struct aw_wifi_status {
    aw_wifi_state_t  state;                 /**< \brief run state */
    aw_wifi_opmode_t last_opmode;           /**< \brief Last operation mode.*/
} aw_wifi_status_t;
/**
 * \brief AP information
 */
typedef struct aw_wifi_ap_info {
    
    /** 
     *\brief Service Set Identification (i.e. Name of Access Point)
     */
    aw_wifi_ssid_t        ssid;             
    
    /**
     *\brief Basic Service Set Identification (i.e. MAC address of Access Point)
     */
     aw_wifi_mac_t         bssid;            
    
    /** 
     *\brief Receive Signal Strength Indication in dBm.
     * <-90=Very poor, >-30=Excellent 
     */
    int16_t               signal_strength;  
    
    /** \brief Maximum data rate in kilobits/s */
    uint32_t              max_data_rate;    
    
    /** \brief Network type */
    aw_wifi_bss_type_t    bss_type;         
    
    /** \brief Security type */
    aw_wifi_security_t    security;         
    
    /** \brief Radio channel that the AP beacon was received on */
    uint8_t               channel;          
    
    /** \brief Radio band */
    aw_wifi_802_11_band_t band;             
} aw_wifi_ap_info_t;
/**
 * \brief STA information
 */
typedef struct aw_wifi_sta_info {
    aw_wifi_mac_t   bssid;            /**< \brief BSSID of STA */
    int             signal_strength;  /**< \brief dbm */
} aw_wifi_sta_info_t;
/**
 * Structure for storing scan results
 */
typedef struct aw_wifi_scan_result {
    
    /** \brief Service Set Identification (i.e. Name of Access Point) */
    aw_wifi_ssid_t              ssid;             
    
    /**
     *\brief Basic Service Set Identification (i.e. MAC address of Access Point) 
     */
    aw_wifi_mac_t               bssid;            
    
    /**
     *\brief Receive Signal Strength Indication in dBm.
     *<-90=Very poor, >-30=Excellent 
     */
    int16_t                     signal_strength;  
    
    /** \brief Maximum data rate in kilobits/s */
    uint32_t                    max_data_rate;    
    
    /** \brief Network type */
    aw_wifi_bss_type_t          bss_type;         
    
    /** \brief Security type */
    aw_wifi_security_t          security;         
    
    /** \brief Radio channel that the AP beacon was received on */
    uint8_t                     channel;          
    
    /** \brief Radio band */
    aw_wifi_802_11_band_t       band;             
    
    /** \brief Two letter ISO country code from AP */
    uint8_t                     ccode[4];         
    
    /** \brief flags */
    uint8_t                     flags;            
    
    /** \brief Pointer to the next scan result */
    struct aw_wifi_scan_result *next;             
} aw_wifi_scan_result_t;
/**
 * Enumeration of methods of scanning
 */
typedef enum aw_wifi_scan_type {
    
    /** \brief Actively scan a network by sending 802.11 probe(s) */
    AW_WIFI_SCAN_TYPE_ACTIVE              = 0,  
    
    /** \brief Passively scan a network by listening for beacons from APs */
    AW_WIFI_SCAN_TYPE_PASSIVE            ,  
    
    /** \brief Use preferred network offload to detect an AP */
    AW_WIFI_SCAN_TYPE_PNO                ,  
    
    /** \brief Passively scan on channels not enabled by the country code */
    AW_WIFI_SCAN_TYPE_PROHIBITED_CHANNELS   
    
} aw_wifi_scan_type_t;
typedef enum aw_wifi_scan_result_flag {
    
    /** \brief RSSI came from an off channel DSSS (1 or 1 Mb) Rx */
    AW_WIFI_SCAN_RESULT_FLAG_RSSI_OFF_CHANNEL   = 0x01,  
    
    /** \brief Beacon (vs probe response) */
    AW_WIFI_SCAN_RESULT_FLAG_BEACON             = 0x02   
} aw_wifi_scan_result_flag_t;
typedef enum aw_wifi_scan_status {
    AW_WIFI_SCAN_INCOMPLETE,
    AW_WIFI_SCAN_COMPLETED_SUCCESSFULLY,
    AW_WIFI_SCAN_ABORTED,
} aw_wifi_scan_status_t;
/**
 * Wi-Fi scan result
 */
typedef struct aw_wifi_scan_cb_result {
    
    /** \brief Access point details */
    aw_wifi_scan_result_t  ap_details;   
    
    /** \brief Pointer to user data passed into aw_wifi_scan() function */
    void                  *p_arg;        
} aw_wifi_scan_cb_result_t;
typedef aw_err_t (*aw_wifi_scan_callback_t) (aw_wifi_scan_cb_result_t *result,
                                             aw_wifi_scan_status_t     status);
typedef aw_err_t (*aw_wifi_get_sta_result_handler_t) (
    void               *p_arg,
    aw_wifi_sta_info_t *p_info,
    aw_bool_t           b_complete);
/**
 * SoftAP events
 */
typedef enum aw_wifi_softap_event {
    
    /** \brief Unknown SoftAP event       */
    AW_WIFI_AP_UNKNOWN_EVENT,         
    
    /** \brief a STA joined our SoftAP    */
    AW_WIFI_AP_STA_JOINED_EVENT,      
    
    /** \brief a STA left our SoftAP      */
    AW_WIFI_AP_STA_LEAVE_EVENT,       
} aw_wifi_ap_event_t;
typedef void (*aw_wifi_ap_event_handler_t) (void                *p_arg,
                                            aw_wifi_ap_event_t   event,
                                            const aw_wifi_mac_t *mac_addr);
typedef void (*aw_wifi_raw_packet_processor_t) (void    *p_arg, 
                                                uint8_t *p_buf, 
                                                size_t   size);
#ifdef __cplusplus
}
#endif
#endif /* __AW_WIFI_TYPES_H */
/* end of file */
