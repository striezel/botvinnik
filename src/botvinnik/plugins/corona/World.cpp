/*
 -------------------------------------------------------------------------------
    This file is part of the botvinnik Matrix bot.
    Copyright (C) 2021  Dirk Stolle

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 -------------------------------------------------------------------------------
*/

#include "World.hpp"
#include <algorithm>
#include <functional>
#include <unordered_map>
#include "../../../util/Strings.hpp"
#include "disease.sh.hpp"

namespace bvn
{

std::vector<Country> World::all()
{
  return {
    { 1, "Afghanistan", "AF", 38041757 },
    { 2, "Albania", "AL", 2862427 },
    { 3, "Algeria", "DZ", 43053054 },
    { 4, "Andorra", "AD", 76177 },
    { 5, "Angola", "AO", 31825299 },
    { 6, "Anguilla", "AI", 14872 },
    { 7, "Antigua and Barbuda", "AG", 97115 },
    { 8, "Argentina", "AR", 44780675 },
    { 9, "Armenia", "AM", 2957728 },
    { 10, "Aruba", "AW", 106310 },
    { 11, "Australia", "AU", 25203200 },
    { 12, "Austria", "AT", 8858775 },
    { 13, "Azerbaijan", "AZ", 10047719 },
    { 14, "Bahamas", "BS", 389486 },
    { 15, "Bahrain", "BH", 1641164 },
    { 16, "Bangladesh", "BD", 163046173 },
    { 17, "Barbados", "BB", 287021 },
    { 18, "Belarus", "BY", 9452409 },
    { 19, "Belgium", "BE", 11455519 },
    { 20, "Belize", "BZ", 390351 },
    { 21, "Benin", "BJ", 11801151 },
    { 22, "Bermuda", "BM", 62508 },
    { 23, "Bhutan", "BT", 763094 },
    { 24, "Bolivia", "BO", 11513102 },
    { 25, "Bonaire, Saint Eustatius and Saba", "BQ", 25983 },
    { 26, "Bosnia and Herzegovina", "BA", 3300998 },
    { 27, "Botswana", "BW", 2303703 },
    { 28, "Brazil", "BR", 211049519 },
    { 29, "British Virgin Islands", "VG", 30033 },
    { 30, "Brunei Darussalam", "BN", 433296 },
    { 31, "Bulgaria", "BG", 7000039 },
    { 32, "Burkina Faso", "BF", 20321383 },
    { 33, "Burundi", "BI", 11530577 },
    { 34, "Cambodia", "KH", 16486542 },
    { 35, "Cameroon", "CM", 25876387 },
    { 36, "Canada", "CA", 37411038 },
    { 37, "Cape Verde", "CV", 549936 },
    { 38, "Cases on an international conveyance Japan", "JPG11668", -1 },
    { 39, "Cayman Islands", "KY", 64948 },
    { 40, "Central African Republic", "CF", 4745179 },
    { 41, "Chad", "TD", 15946882 },
    { 42, "Chile", "CL", 18952035 },
    { 43, "China", "CN", 1433783692 },
    { 44, "Colombia", "CO", 50339443 },
    { 45, "Comoros", "KM", 850891 },
    { 46, "Congo", "CG", 5380504 },
    { 215, "Cook Islands", "CK", 17459 },
    { 47, "Costa Rica", "CR", 5047561 },
    { 48, "Cote d'Ivoire", "CI", 25716554 },
    { 49, "Croatia", "HR", 4076246 },
    { 50, "Cuba", "CU", 11333484 },
    { 51, "Curaçao", "CW", 163423 },
    { 52, "Cyprus", "CY", 875899 },
    { 53, "Czechia", "CZ", 10649800 },
    { 54, "Democratic Republic of the Congo", "CD", 86790568 },
    { 55, "Denmark", "DK", 5806081 },
    { 56, "Djibouti", "DJ", 973557 },
    { 57, "Dominica", "DM", 71808 },
    { 58, "Dominican Republic", "DO", 10738957 },
    { 59, "Ecuador", "EC", 17373657 },
    { 60, "Egypt", "EG", 100388076 },
    { 61, "El Salvador", "SV", 6453550 },
    { 62, "Equatorial Guinea", "GQ", 1355982 },
    { 63, "Eritrea", "ER", 3497117 },
    { 64, "Estonia", "EE", 1324820 },
    { 65, "Eswatini", "SZ", 1148133 },
    { 66, "Ethiopia", "ET", 112078727 },
    { 67, "Falkland Islands (Malvinas)", "FK", 3372 },
    { 68, "Faroe Islands", "FO", 48677 },
    { 69, "Fiji", "FJ", 889955 },
    { 70, "Finland", "FI", 5517919 },
    { 71, "France", "FR", 67012883 },
    { 72, "French Polynesia", "PF", 279285 },
    { 73, "Gabon", "GA", 2172578 },
    { 74, "Gambia", "GM", 2347696 },
    { 75, "Georgia", "GE", 3996762 },
    { 76, "Germany", "DE", 83019213 },
    { 77, "Ghana", "GH", 30417858 },
    { 78, "Gibraltar", "GI", 33706 },
    { 79, "Greece", "EL", 10724599 },
    { 80, "Greenland", "GL", 56660 },
    { 81, "Grenada", "GD", 112002 },
    { 82, "Guam", "GU", 167295 },
    { 83, "Guatemala", "GT", 17581476 },
    //{ 84, "Guernsey", "GG", 64468 },
    { 85, "Guinea", "GN", 12771246 },
    { 86, "Guinea Bissau", "GW", 1920917 },
    { 87, "Guyana", "GY", 782775 },
    { 88, "Haiti", "HT", 11263079 },
    { 89, "Holy See", "VA", 815 },
    { 90, "Honduras", "HN", 9746115 },
    { 91, "Hungary", "HU", 9772756 },
    { 92, "Iceland", "IS", 356991 },
    { 93, "India", "IN", 1366417756 },
    { 94, "Indonesia", "ID", 270625567 },
    { 95, "Iran", "IR", 82913893 },
    { 96, "Iraq", "IQ", 39309789 },
    { 97, "Ireland", "IE", 4904240 },
    { 98, "Isle of Man", "IM", 84589 },
    { 99, "Israel", "IL", 8519373 },
    { 100, "Italy", "IT", 60359546 },
    { 101, "Jamaica", "JM", 2948277 },
    { 102, "Japan", "JP", 126860299 },
    //{ 103, "Jersey", "JE", 107796 },
    { 104, "Jordan", "JO", 10101697 },
    { 105, "Kazakhstan", "KZ", 18551428 },
    { 106, "Kenya", "KE", 52573967 },
    { 107, "Kosovo", "XK", 1798506 },
    { 108, "Kuwait", "KW", 4207077 },
    { 109, "Kyrgyzstan", "KG", 6415851 },
    { 110, "Laos", "LA", 7169456 },
    { 111, "Latvia", "LV", 1919968 },
    { 112, "Lebanon", "LB", 6855709 },
    { 113, "Lesotho", "LS", 2125267 },
    { 114, "Liberia", "LR", 4937374 },
    { 115, "Libya", "LY", 6777453 },
    { 116, "Liechtenstein", "LI", 38378 },
    { 117, "Lithuania", "LT", 2794184 },
    { 118, "Luxembourg", "LU", 613894 },
    { 119, "Madagascar", "MG", 26969306 },
    { 120, "Malawi", "MW", 18628749 },
    { 121, "Malaysia", "MY", 31949789 },
    { 122, "Maldives", "MV", 530957 },
    { 123, "Mali", "ML", 19658023 },
    { 124, "Malta", "MT", 493559 },
    { 125, "Marshall Islands", "MH", 58791 },
    { 126, "Mauritania", "MR", 4525698 },
    { 127, "Mauritius", "MU", 1269670 },
    { 128, "Mexico", "MX", 127575529 },
    { 129, "Moldova", "MD", 4043258 },
    { 130, "Monaco", "MC", 33085 },
    { 131, "Mongolia", "MN", 3225166 },
    { 132, "Montenegro", "ME", 622182 },
    { 133, "Montserrat", "MS", 4991 },
    { 134, "Morocco", "MA", 36471766 },
    { 135, "Mozambique", "MZ", 30366043 },
    { 136, "Myanmar", "MM", 54045422 },
    { 137, "Namibia", "NA", 2494524 },
    { 138, "Nepal", "NP", 28608715 },
    { 139, "Netherlands", "NL", 17282163 },
    { 140, "New Caledonia", "NC", 282757 },
    { 141, "New Zealand", "NZ", 4783062 },
    { 142, "Nicaragua", "NI", 6545503 },
    { 143, "Niger", "NE", 23310719 },
    { 144, "Nigeria", "NG", 200963603 },
    { 145, "North Macedonia", "MK", 2077132 },
    { 146, "Northern Mariana Islands", "MP", 57213 },
    { 147, "Norway", "NO", 5328212 },
    { 148, "Oman", "OM", 4974992 },
    { 149, "Pakistan", "PK", 216565317 },
    { 150, "Palestine", "PS", 4981422 },
    { 151, "Panama", "PA", 4246440 },
    { 152, "Papua New Guinea", "PG", 8776119 },
    { 153, "Paraguay", "PY", 7044639 },
    { 154, "Peru", "PE", 32510462 },
    { 155, "Philippines", "PH", 108116622 },
    { 156, "Poland", "PL", 37972812 },
    { 157, "Portugal", "PT", 10276617 },
    { 158, "Puerto Rico", "PR", 2933404 },
    { 159, "Qatar", "QA", 2832071 },
    { 160, "Romania", "RO", 19414458 },
    { 161, "Russia", "RU", 145872260 },
    { 162, "Rwanda", "RW", 12626938 },
    { 163, "Saint Kitts and Nevis", "KN", 52834 },
    { 164, "Saint Lucia", "LC", 182795 },
    { 165, "Saint Vincent and the Grenadines", "VC", 110593 },
    { 166, "San Marino", "SM", 34453 },
    { 167, "Sao Tome and Principe", "ST", 215048 },
    { 168, "Saudi Arabia", "SA", 34268529 },
    { 169, "Senegal", "SN", 16296362 },
    { 170, "Serbia", "RS", 6963764 },
    { 171, "Seychelles", "SC", 97741 },
    { 172, "Sierra Leone", "SL", 7813207 },
    { 173, "Singapore", "SG", 5804343 },
    { 174, "Sint Maarten", "SX", 42389 },
    { 175, "Slovakia", "SK", 5450421 },
    { 176, "Slovenia", "SI", 2080908 },
    { 177, "Solomon Islands", "SB", 669821 },
    { 178, "Somalia", "SO", 15442906 },
    { 179, "South Africa", "ZA", 58558267 },
    { 180, "South Korea", "KR", 51225321 },
    { 181, "South Sudan", "SS", 11062114 },
    { 182, "Spain", "ES", 46937060 },
    { 183, "Sri Lanka", "LK", 21323734 },
    { 184, "Sudan", "SD", 42813237 },
    { 185, "Suriname", "SR", 581363 },
    { 186, "Sweden", "SE", 10230185 },
    { 187, "Switzerland", "CH", 8544527 },
    { 188, "Syria", "SY", 17070132 },
    { 189, "Taiwan", "TW", 23773881 },
    { 190, "Tajikistan", "TJ", 9321023 },
    { 191, "Thailand", "TH", 69625581 },
    { 192, "Timor Leste", "TL", 1293120 },
    { 193, "Togo", "TG", 8082359 },
    { 194, "Trinidad and Tobago", "TT", 1394969 },
    { 195, "Tunisia", "TN", 11694721 },
    { 196, "Turkey", "TR", 82003882 },
    { 197, "Turks and Caicos islands", "TC", 38194 },
    { 198, "Uganda", "UG", 44269587 },
    { 199, "Ukraine", "UA", 43993643 },
    { 200, "United Arab Emirates", "AE", 9770526 },
    { 201, "United Kingdom", "UK", 66647112 },
    { 202, "United Republic of Tanzania", "TZ", 58005461 },
    { 203, "United States of America", "US", 329064917 },
    { 204, "United States Virgin Islands", "VI", 104579 },
    { 205, "Uruguay", "UY", 3461731 },
    { 206, "Uzbekistan", "UZ", 32981715 },
    { 207, "Vanuatu", "VU", 299882 },
    { 208, "Venezuela", "VE", 28515829 },
    { 209, "Vietnam", "VN", 96462108 },
    { 210, "Wallis and Futuna", "WF", -1 },
    { 211, "Western Sahara", "EH", 582458 },
    { 212, "Yemen", "YE", 29161922 },
    { 213, "Zambia", "ZM", 17861034 },
    { 214, "Zimbabwe", "ZW", 14645473 }
  };
}

std::optional<Country> World::find(const std::string& nameOrGeoId)
{
  const auto lower = toLowerString(nameOrGeoId);
  const auto countries = World::all();
  auto predicate = [&] (const Country& c) {
    return toLowerString(c.geoId) == lower || lower == toLowerString(c.name);
  };
  const auto iter = std::find_if(countries.begin(), countries.end(), predicate);
  if (iter != countries.end())
    return std::optional<Country>(*iter);
  // No match found.
  return std::optional<Country>();
}

// std::function<CovidNumbers(const Country&)>
const std::unordered_map<std::string, std::function<CovidNumbers(const Country&)>> data_functions = {
  // Anguilla
  { "AI", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "Anguilla", false); } },
  // Aruba
  { "AW", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("NL", "Aruba", false); } },
  // Bermuda
  { "BM", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "Bermuda", false); } },
  // Bonaire, Saint Eustatius and Saba
  { "BQ", [] (const Country& c) { return disease_sh::requestHistoricalApiFirstOfMultipleProvinces("NL", "bonaire%2C%20sint%20eustatius%20and%20saba%7C", false); } },
  // Cook Islands
  { "CK", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("NZ", "cook%20islands", false); } },
  // Curacao
  { "CW", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("NL", "Curacao", false); } },
  // Denmark
  { "DK", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("DK", "mainland", false); } },
  // Falkland Islands
  { "FK", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "falkland%20islands%20(malvinas)", false); } },
  // Faroe Islands
  { "FO", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("DK", "faroe%20islands", false); } },
  // France
  { "FR", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("FR", "mainland", false); } },
  // Gibraltar
  { "GI", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "gibraltar", false); } },
  // Greenland
  { "GL", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("DK", "Greenland", false); } },
  // Guam
  { "GU", [] (const Country& c) { return disease_sh::requestHistoricalApiUsaCounties("guam", false); } },
  // Isle of Man
  { "IM", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "isle%20of%20man", false); } },
  // Cayman Islands
  { "KY", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "Cayman%20Islands", false); } },
  // Myanmar
  { "MM", [] (const Country& c) { return disease_sh::requestHistoricalApi("BU", false); } },
  // Montserrat
  { "MS", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "Montserrat", false); } },
  // New Caledonia
  { "NC", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("FR", "new%20caledonia", false); } },
  // Netherlands
  { "NL", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("NL", "mainland", false); } },
  // New Zealand
  { "NZ", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("NZ", "mainland", false); } },
  // French Polynesia
  { "PF", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("FR", "french%20polynesia", false); } },
  // Puerto Rico
  { "PR", [] (const Country& c) { return disease_sh::requestHistoricalApiUsaCounties("puerto%20rico", false); } },
  // Palestine
  { "PS", [] (const Country& c) { return disease_sh::requestHistoricalApi("West%20Bank%20and%20Gaza", false); } },
  // Sint Maarten
  { "SX", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("NL", "sint%20maarten", false); } },
  // Turks and Caicos Islands
  { "TC", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "turks%20and%20caicos%20islands", false); } },
  // United Kingdom
  { "UK", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "mainland", false); } },
  // Vatican City
  { "VA", [] (const Country& c) { return disease_sh::requestHistoricalApi("Holy%20See", false); } },
  // British Virgin Islands
  { "VG", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("UK", "British%20Virgin%20Islands", false); } },
  // United States Virgin Islands
  { "VI", [] (const Country& c) { return disease_sh::requestHistoricalApiUsaCounties("virgin%20islands", false); } },
  // Wallis and Futuna
  { "WF", [] (const Country& c) { return disease_sh::requestHistoricalApiProvince("FR", "wallis%20and%20futuna", false); } }
};

CovidNumbers World::getCountryData(const Country& country)
{
  const auto iter = data_functions.find(country.geoId);
  if (iter != data_functions.end())
  {
    return iter->second(country);
  }
  // Default method: Just call API with geo id of country.
  return disease_sh::requestHistoricalApi(country.geoId, false);
}

} // namespace
