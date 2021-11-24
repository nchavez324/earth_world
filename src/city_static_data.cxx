#include "city_static_data.h"

namespace earth_world {

CityStaticData::CityStaticData(std::string name, std::string country_name,
                               SpherePoint2 location)
    : name_{name}, country_name_{country_name}, location_{location} {}

const std::string& CityStaticData::getName() const { return name_; }

const std::string& CityStaticData::getCountryName() const {
  return country_name_;
}

const SpherePoint2& CityStaticData::getLocation() const { return location_; }

std::vector<CityStaticData> kDefaultCities = {
    CityStaticData(
        "New York", "USA",
        SpherePoint2::fromLatitudeAndLongitude(40.712776f, -74.005974f)),
    CityStaticData(
        "Boston", "USA",
        SpherePoint2::fromLatitudeAndLongitude(42.359903f, -71.058447f)),
    CityStaticData(
        "Washington D.C.", "USA",
        SpherePoint2::fromLatitudeAndLongitude(38.907147f, -77.036453f)),
    CityStaticData(
        "Miami", "USA",
        SpherePoint2::fromLatitudeAndLongitude(25.761832f, -80.191629f)),
    CityStaticData(
        "Nassau", "Bahamas",
        SpherePoint2::fromLatitudeAndLongitude(25.044238f, -77.350199f)),
    CityStaticData(
        "Havana", "Cuba",
        SpherePoint2::fromLatitudeAndLongitude(23.113568f, -82.366425f)),
    CityStaticData(
        "Santo Domingo", "Dominican Republic",
        SpherePoint2::fromLatitudeAndLongitude(18.486018f, -69.931417f)),
    CityStaticData(
        "Kingston", "Jamaica",
        SpherePoint2::fromLatitudeAndLongitude(18.017859f, -76.809942f)),
    CityStaticData(
        "San Juan", "Puerto Rico",
        SpherePoint2::fromLatitudeAndLongitude(18.465600f, -66.105204f)),
    CityStaticData(
        "New Orleans", "USA",
        SpherePoint2::fromLatitudeAndLongitude(29.951046f, -90.071463f)),
    CityStaticData(
        "Houston", "USA",
        SpherePoint2::fromLatitudeAndLongitude(29.760426f, -95.369771f)),
    CityStaticData(
        "Veracruz", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(19.173910f, -96.134067f)),
    CityStaticData(
        "Cancun", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(21.161881f, -86.851574f)),
    CityStaticData(
        "Belize City", "Belize",
        SpherePoint2::fromLatitudeAndLongitude(17.504275f, -88.196351f)),
    CityStaticData(
        "San Pedro Sula", "Honduras",
        SpherePoint2::fromLatitudeAndLongitude(15.503914f, -88.013879f)),
    CityStaticData(
        "San Salvador", "El Salvador",
        SpherePoint2::fromLatitudeAndLongitude(13.692922f, -89.218090f)),
    CityStaticData(
        "Managua", "Nicaragua",
        SpherePoint2::fromLatitudeAndLongitude(12.114764f, -86.236012f)),
    CityStaticData(
        "San Jose", "Costa Rica",
        SpherePoint2::fromLatitudeAndLongitude(9.927598f, -84.090610f)),
    CityStaticData(
        "Panama City", "Panama",
        SpherePoint2::fromLatitudeAndLongitude(8.982239f, -79.519576f)),
    CityStaticData(
        "Acapulco", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(16.853047f, -99.823477f)),
    CityStaticData(
        "Puerto Vallarta", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(20.653413f, -105.225242f)),
    CityStaticData(
        "Cabo San Lucas", "Mexico",
        SpherePoint2::fromLatitudeAndLongitude(22.890660f, -109.916962f)),
    CityStaticData(
        "San Diego", "USA",
        SpherePoint2::fromLatitudeAndLongitude(32.715562f, -117.161681f)),
    CityStaticData(
        "Los Angeles", "USA",
        SpherePoint2::fromLatitudeAndLongitude(34.052197f, -118.243390f)),
    CityStaticData(
        "San Francisco", "USA",
        SpherePoint2::fromLatitudeAndLongitude(37.775018f, -122.419272f)),
    CityStaticData(
        "Seattle", "USA",
        SpherePoint2::fromLatitudeAndLongitude(47.606086f, -122.332885f)),
    CityStaticData(
        "Vancouver", "Canada",
        SpherePoint2::fromLatitudeAndLongitude(49.282561f, -123.120550f)),
    CityStaticData(
        "Anchorage", "USA",
        SpherePoint2::fromLatitudeAndLongitude(61.218058f, -149.899900f)),
    CityStaticData(
        "Buenaventura", "Colombia",
        SpherePoint2::fromLatitudeAndLongitude(3.882920f, -77.019631f)),
    CityStaticData(
        "Cartagena", "Colombia",
        SpherePoint2::fromLatitudeAndLongitude(10.393273f, -75.483010f)),
    CityStaticData(
        "Guayaquil", "Ecuador",
        SpherePoint2::fromLatitudeAndLongitude(-2.189239f, -79.889122f)),
    CityStaticData(
        "Lima", "Peru",
        SpherePoint2::fromLatitudeAndLongitude(-12.046434, -77.042566f)),
    CityStaticData(
        "Valparaíso", "Chile",
        SpherePoint2::fromLatitudeAndLongitude(-33.047281f, -71.612747f)),
    CityStaticData(
        "Buenos Aires", "Argentina",
        SpherePoint2::fromLatitudeAndLongitude(-34.603652f, -58.381354f)),
    CityStaticData(
        "Montevideo", "Uruguay",
        SpherePoint2::fromLatitudeAndLongitude(-34.901034f, -56.164223f)),
    CityStaticData(
        "Barranquilla", "Colombia",
        SpherePoint2::fromLatitudeAndLongitude(11.004403f, -74.807123f)),
    CityStaticData(
        "Maracaibo", "Venezuela",
        SpherePoint2::fromLatitudeAndLongitude(10.642597f, -71.611861f)),
    CityStaticData(
        "Caracas", "Venezuela",
        SpherePoint2::fromLatitudeAndLongitude(10.480894f, -66.903729f)),
    CityStaticData(
        "Quebec City", "Canada",
        SpherePoint2::fromLatitudeAndLongitude(46.812579f, -71.206311f)),
    CityStaticData(
        "Georgetown", "Guyana",
        SpherePoint2::fromLatitudeAndLongitude(6.801656f, -58.155585)),
    CityStaticData(
        "Paramaribo", "Suriname",
        SpherePoint2::fromLatitudeAndLongitude(5.852355f, -55.203619f)),
    CityStaticData(
        "Cayenne", "French Guiana",
        SpherePoint2::fromLatitudeAndLongitude(4.922331f, -52.313706f)),
    CityStaticData(
        "Belem", "Brazil",
        SpherePoint2::fromLatitudeAndLongitude(-1.456363f, -48.501220f)),
    CityStaticData(
        "São Luís", "Brazil",
        SpherePoint2::fromLatitudeAndLongitude(-2.530643f, -44.299612f)),
    CityStaticData(
        "Fortaleza", "Brazil",
        SpherePoint2::fromLatitudeAndLongitude(-3.731864f, -38.526160f)),
    CityStaticData(
        "Recife", "Brazil",
        SpherePoint2::fromLatitudeAndLongitude(-8.057606f, -34.882902f)),
    CityStaticData(
        "Salvador", "Brazil",
        SpherePoint2::fromLatitudeAndLongitude(-12.977872f, -38.501370f)),
    CityStaticData(
        "Rio de Janeiro", "Brazil",
        SpherePoint2::fromLatitudeAndLongitude(-22.906714f, -43.173073f)),
    CityStaticData(
        "São Paulo", "Brazil",
        SpherePoint2::fromLatitudeAndLongitude(-23.545528f, -46.631882f)),
    CityStaticData(
        "Stanley", "United Kingdom",
        SpherePoint2::fromLatitudeAndLongitude(-51.696269f, -57.851568f)),
    CityStaticData(
        "Punta Arenas", "Chile",
        SpherePoint2::fromLatitudeAndLongitude(-53.164013f, -70.917144f)),
    CityStaticData(
        "Nuuk", "Greenland",
        SpherePoint2::fromLatitudeAndLongitude(64.181417f, -51.694035f)),
    CityStaticData(
        "St. John's", "Canada",
        SpherePoint2::fromLatitudeAndLongitude(-47.561302f, -52.712180f)),
    CityStaticData(
        "Reykjavik", "Iceland",
        SpherePoint2::fromLatitudeAndLongitude(64.146668f, -21.942496f)),
    CityStaticData(
        "Dublin", "Ireland",
        SpherePoint2::fromLatitudeAndLongitude(53.349706f, -6.260038f)),
    CityStaticData(
        "Londin", "United Kingdom",
        SpherePoint2::fromLatitudeAndLongitude(51.507233f, -0.127856f)),
    CityStaticData(
        "Liverpool", "United Kingdom",
        SpherePoint2::fromLatitudeAndLongitude(53.408333f, -2.991651f)),
    CityStaticData(
        "Edinburgh", "United Kingdom",
        SpherePoint2::fromLatitudeAndLongitude(55.953499f, -3.188464f)),
    CityStaticData(
        "Paris", "France",
        SpherePoint2::fromLatitudeAndLongitude(48.854684f, 2.347693f)),
    CityStaticData(
        "Calais", "France",
        SpherePoint2::fromLatitudeAndLongitude(50.951458f, 1.859096f)),
    CityStaticData(
        "Le Havre", "France",
        SpherePoint2::fromLatitudeAndLongitude(49.494039f, 0.108561f)),
    CityStaticData(
        "Brest", "France",
        SpherePoint2::fromLatitudeAndLongitude(48.390274f, -4.486294f)),
    CityStaticData(
        "Bordeaux", "France",
        SpherePoint2::fromLatitudeAndLongitude(44.837635f, -0.578992f)),
    CityStaticData(
        "Marseille", "France",
        SpherePoint2::fromLatitudeAndLongitude(43.296793f, 5.369557f)),
    CityStaticData(
        "Nice", "France",
        SpherePoint2::fromLatitudeAndLongitude(43.710133f, 7.262014f)),
    CityStaticData("Ajaccio", "France",
                   SpherePoint2::fromLatitudeAndLongitude(41.919077f,
                                                          8.738726f)),
    CityStaticData(
        "Bilbao", "Spain",
        SpherePoint2::fromLatitudeAndLongitude(43.262949f, -2.934888f)),
    CityStaticData(
        "Porto", "Portugal",
        SpherePoint2::fromLatitudeAndLongitude(41.157606f, -8.628755f)),
    CityStaticData(
        "Gibraltar", "United Kingdom",
        SpherePoint2::fromLatitudeAndLongitude(36.140589f, -5.353628f)),
    CityStaticData(
        "Barcelona", "Spain",
        SpherePoint2::fromLatitudeAndLongitude(41.386703f, 2.170979f)),
    CityStaticData(
        "Palma", "Spain",
        SpherePoint2::fromLatitudeAndLongitude(39.569536f, 2.650229f)),
    CityStaticData(
        "Genoa", "Italy",
        SpherePoint2::fromLatitudeAndLongitude(44.405549f, 8.946556f)),
    CityStaticData(
        "Cagliari", "Italy",
        SpherePoint2::fromLatitudeAndLongitude(39.223798f, 9.121779f)),
    CityStaticData("Palermo", "Italy",
                   SpherePoint2::fromLatitudeAndLongitude(38.115838f,
                                                          13.361865f)),
    CityStaticData(
        "Rome", "Italy",
        SpherePoint2::fromLatitudeAndLongitude(41.902790f, 12.496469f)),
    CityStaticData(
        "Naples", "Italy",
        SpherePoint2::fromLatitudeAndLongitude(40.851673f, 14.268083f)),
    CityStaticData(
        "Venice", "Italy",
        SpherePoint2::fromLatitudeAndLongitude(45.441128f, 12.314982f)),
    CityStaticData(
        "Rijeka", "Croatia",
        SpherePoint2::fromLatitudeAndLongitude(45.326834f, 14.442173f)),
    CityStaticData(
        "Corfu", "Greece",
        SpherePoint2::fromLatitudeAndLongitude(39.625077f, 19.922484f)),
    CityStaticData(
        "Athens", "Greece",
        SpherePoint2::fromLatitudeAndLongitude(37.983808f, 23.727912f)),
    CityStaticData(
        "Heraklion", "Greece",
        SpherePoint2::fromLatitudeAndLongitude(35.338658f, 25.144312f)),
    CityStaticData(
        "Rhodes", "Greece",
        SpherePoint2::fromLatitudeAndLongitude(36.435885f, 28.217290f)),
    CityStaticData(
        "Thessaloniki", "Greece",
        SpherePoint2::fromLatitudeAndLongitude(40.638800f, 22.945140f)),
    CityStaticData("Nicosia", "Cyprus",
                   SpherePoint2::fromLatitudeAndLongitude(35.185421f,
                                                          33.382309f)),
    CityStaticData(
        "Valletta", "Malta",
        SpherePoint2::fromLatitudeAndLongitude(35.898870f, 14.514464f)),
    CityStaticData(
        "Izmir", "Turkey",
        SpherePoint2::fromLatitudeAndLongitude(38.423536f, 27.143218f)),
    CityStaticData(
        "Istanbul", "Turkey",
        SpherePoint2::fromLatitudeAndLongitude(41.008207f, 28.978278f)),
    CityStaticData("Antalya", "Turkey",
                   SpherePoint2::fromLatitudeAndLongitude(36.896915f,
                                                          30.713467f)),
    CityStaticData(
        "Varna", "Bulgaria",
        SpherePoint2::fromLatitudeAndLongitude(43.213982f, 27.914884f)),
    CityStaticData(
        "Odesa", "Ukraine",
        SpherePoint2::fromLatitudeAndLongitude(46.482354f, 30.722819f)),
    CityStaticData(
        "Sevastopol", "Russia",
        SpherePoint2::fromLatitudeAndLongitude(44.616600f, 33.524797f)),
    CityStaticData("Antwerp", "Belgium",
                   SpherePoint2::fromLatitudeAndLongitude(51.219563f,
                                                          4.402725f)),
    CityStaticData(
        "Amsterdam", "Netherlands",
        SpherePoint2::fromLatitudeAndLongitude(52.368058f, 4.904446f)),
    CityStaticData("Hamburg", "Germany",
                   SpherePoint2::fromLatitudeAndLongitude(53.551710,
                                                          9.994482f)),
    CityStaticData(
        "Copenhagen", "Denmark",
        SpherePoint2::fromLatitudeAndLongitude(55.675894f, 12.568035f)),
    CityStaticData(
        "Oslo", "Norway",
        SpherePoint2::fromLatitudeAndLongitude(59.913766f, 10.752748f)),
    CityStaticData(
        "Longyearbyen", "Norway",
        SpherePoint2::fromLatitudeAndLongitude(78.223055f, 15.627158f)),
    CityStaticData(
        "Gothenburg", "Sweden",
        SpherePoint2::fromLatitudeAndLongitude(57.708580f, 11.974629f)),
    CityStaticData(
        "Stockholm", "Sweden",
        SpherePoint2::fromLatitudeAndLongitude(59.329106f, 18.068547f)),
    CityStaticData(
        "Oulu", "Finland",
        SpherePoint2::fromLatitudeAndLongitude(65.012073f, 25.465170f)),
    CityStaticData(
        "Helsinki", "Finland",
        SpherePoint2::fromLatitudeAndLongitude(60.169892f, 24.938507f)),
    CityStaticData(
        "St. Petersburg", "Russia",
        SpherePoint2::fromLatitudeAndLongitude(59.930969f, 30.361374f)),
    CityStaticData("Tallinn", "Estonia",
                   SpherePoint2::fromLatitudeAndLongitude(59.436879f,
                                                          24.753620f)),
    CityStaticData(
        "Riga", "Latvia",
        SpherePoint2::fromLatitudeAndLongitude(56.948487f, 24.105348f)),
    CityStaticData(
        "Klaipeda", "Lithuania",
        SpherePoint2::fromLatitudeAndLongitude(55.703037f, 21.143817f)),
    CityStaticData(
        "Kaliningrad", "Russia",
        SpherePoint2::fromLatitudeAndLongitude(54.710129f, 20.452223f)),
    CityStaticData(
        "Gdansk", "Poland",
        SpherePoint2::fromLatitudeAndLongitude(54.351809f, 18.646598f)),
    CityStaticData(
        "Szczecin", "Poland",
        SpherePoint2::fromLatitudeAndLongitude(53.428565f, 14.553061f)),
    CityStaticData(
        "Beirut", "Lebanon",
        SpherePoint2::fromLatitudeAndLongitude(33.893652f, 35.501358f)),
    CityStaticData(
        "Tel-Aviv", "Israel",
        SpherePoint2::fromLatitudeAndLongitude(32.085327f, 34.781774f)),
    CityStaticData(
        "Alexandria", "Egypt",
        SpherePoint2::fromLatitudeAndLongitude(31.199918f, 29.918800f)),
    CityStaticData(
        "Cairo", "Egypt",
        SpherePoint2::fromLatitudeAndLongitude(30.044364f, 31.235511f)),
    CityStaticData("Tripoli", "Libya",
                   SpherePoint2::fromLatitudeAndLongitude(32.887179f,
                                                          13.191508f)),
    CityStaticData(
        "Benghazi", "Libya",
        SpherePoint2::fromLatitudeAndLongitude(32.119319f, 20.086824f)),
    CityStaticData(
        "Tunis", "Tunisia",
        SpherePoint2::fromLatitudeAndLongitude(36.806227f, 10.181542f)),
    CityStaticData("Algiers", "Algeria",
                   SpherePoint2::fromLatitudeAndLongitude(36.732288f,
                                                          3.087480f)),
    CityStaticData(
        "Casablanca", "Morocco",
        SpherePoint2::fromLatitudeAndLongitude(33.572708f, -7.589739f)),
    CityStaticData(
        "Nouakchott", "Mauritania",
        SpherePoint2::fromLatitudeAndLongitude(18.072551f, -15.959235f)),
    CityStaticData(
        "Ponta Delgada", "Portugal",
        SpherePoint2::fromLatitudeAndLongitude(37.739148f, -25.668274f)),
    CityStaticData(
        "Santa Cruz de Tenerife", "Spain",
        SpherePoint2::fromLatitudeAndLongitude(28.464582f, -16.251608f)),
    CityStaticData(
        "Praia", "Cabo Verde",
        SpherePoint2::fromLatitudeAndLongitude(14.932945f, -23.513208f)),
    CityStaticData(
        "Dakar", "Sengal",
        SpherePoint2::fromLatitudeAndLongitude(14.717115f, -17.467353f)),
    CityStaticData(
        "Bissau", "Guinea-Bissau",
        SpherePoint2::fromLatitudeAndLongitude(11.863130f, -15.583885f)),
    CityStaticData("Conakry", "Guinea",
                   SpherePoint2::fromLatitudeAndLongitude(9.640664f,
                                                          -13.576661f)),
    CityStaticData(
        "Banjul", "The Gambia",
        SpherePoint2::fromLatitudeAndLongitude(13.454368f, -16.576319f)),
    CityStaticData(
        "Freetown", "Sierra Leone",
        SpherePoint2::fromLatitudeAndLongitude(8.466993f, -13.234606f)),
    CityStaticData(
        "Monrovia", "Liberia",
        SpherePoint2::fromLatitudeAndLongitude(6.315572f, -10.807355f)),
    CityStaticData("Abidjan", "Cote d'Ivoire",
                   SpherePoint2::fromLatitudeAndLongitude(5.359575f,
                                                          -4.012431f)),
    CityStaticData(
        "Accra", "Ghana",
        SpherePoint2::fromLatitudeAndLongitude(5.603410f, -0.187324f)),
    CityStaticData(
        "Lome", "Togo",
        SpherePoint2::fromLatitudeAndLongitude(6.124681f, 1.226877f)),
    CityStaticData(
        "Lagos", "Nigeria",
        SpherePoint2::fromLatitudeAndLongitude(6.524033f, 3.382203f)),
    CityStaticData(
        "Douala", "Cameroon",
        SpherePoint2::fromLatitudeAndLongitude(4.049927f, 9.770120f)),
    CityStaticData(
        "Libreville", "Gabon",
        SpherePoint2::fromLatitudeAndLongitude(0.409280f, 9.469082f)),
    CityStaticData(
        "Kinshasa", "Democratic Republic of the Congo",
        SpherePoint2::fromLatitudeAndLongitude(-4.441211f, 15.265467f)),
    CityStaticData(
        "Luanda", "Angola",
        SpherePoint2::fromLatitudeAndLongitude(-8.816115f, 13.230146f)),
    CityStaticData(
        "Cape Town", "South Africa",
        SpherePoint2::fromLatitudeAndLongitude(-33.924524f, 18.426487f)),
    CityStaticData(
        "Durban", "South Africa",
        SpherePoint2::fromLatitudeAndLongitude(-29.858718f, 31.021890f)),
    CityStaticData(
        "Maputo", "Mozambique",
        SpherePoint2::fromLatitudeAndLongitude(-25.969363f, 32.573110f)),
    CityStaticData(
        "Antananarivo", "Madagascar",
        SpherePoint2::fromLatitudeAndLongitude(-18.882432f, 47.507370f)),
    CityStaticData(
        "Zanzibar", "Tanzania",
        SpherePoint2::fromLatitudeAndLongitude(-6.165213f, 39.198904f)),
    CityStaticData(
        "Dar es Salaam", "Tanzania",
        SpherePoint2::fromLatitudeAndLongitude(-6.792678f, 39.208427f)),
    CityStaticData("Mombasa", "Kenya",
                   SpherePoint2::fromLatitudeAndLongitude(-4.043149f,
                                                          39.667890f)),
    CityStaticData(
        "Mogadishu", "Somalia",
        SpherePoint2::fromLatitudeAndLongitude(2.047726f, 45.316060f)),
    CityStaticData(
        "Djibouti", "Djibouti",
        SpherePoint2::fromLatitudeAndLongitude(11.571707f, 43.144874f)),
    CityStaticData(
        "Port Sudan", "Sudan",
        SpherePoint2::fromLatitudeAndLongitude(19.590127f, 37.189917f)),
    CityStaticData(
        "Jeddah", "Saudi Arabia",
        SpherePoint2::fromLatitudeAndLongitude(21.485679f, 39.191261f)),
    CityStaticData(
        "Aden", "Yemen",
        SpherePoint2::fromLatitudeAndLongitude(12.787819f, 45.018306f)),
    CityStaticData(
        "Muscat", "Oman",
        SpherePoint2::fromLatitudeAndLongitude(23.588306f, 58.383312f)),
    CityStaticData(
        "Dubai", "United Arab Emirates",
        SpherePoint2::fromLatitudeAndLongitude(25.204538f, 55.270899f)),
    CityStaticData(
        "Abu Dhabi", "United Arab Emirates",
        SpherePoint2::fromLatitudeAndLongitude(24.453767f, 54.377617f)),
    CityStaticData(
        "Doha", "Qatar",
        SpherePoint2::fromLatitudeAndLongitude(25.285931f, 51.532864f)),
    CityStaticData(
        "Kuwait City", "Kuwait",
        SpherePoint2::fromLatitudeAndLongitude(29.375906f, 47.977114f)),
    CityStaticData(
        "Basrah", "Iraq",
        SpherePoint2::fromLatitudeAndLongitude(30.526122f, 47.773878f)),
    CityStaticData(
        "Bandar Bushehr", "Iran",
        SpherePoint2::fromLatitudeAndLongitude(28.922687f, 50.818794f)),
    CityStaticData("Karachi", "Pakistan",
                   SpherePoint2::fromLatitudeAndLongitude(24.860284f,
                                                          67.000701f)),
    CityStaticData(
        "Mumbai", "India",
        SpherePoint2::fromLatitudeAndLongitude(19.078187f, 72.877968f)),
    CityStaticData(
        "Kochi", "India",
        SpherePoint2::fromLatitudeAndLongitude(9.931781f, 76.267595f)),
    CityStaticData("Colombo", "Sri Lanka",
                   SpherePoint2::fromLatitudeAndLongitude(6.927280f,
                                                          79.861937f)),
    CityStaticData("Chennai", "India",
                   SpherePoint2::fromLatitudeAndLongitude(13.081431f,
                                                          80.272170f)),
    CityStaticData(
        "Visakhapatnam", "India",
        SpherePoint2::fromLatitudeAndLongitude(17.686709f, 83.217984f)),
    CityStaticData("Kolkata", "India",
                   SpherePoint2::fromLatitudeAndLongitude(22.572632f,
                                                          88.362101f)),
    CityStaticData(
        "Dhaka", "Bangladesh",
        SpherePoint2::fromLatitudeAndLongitude(23.811862f, 90.410767f)),
    CityStaticData(
        "Chattogram", "Bangladesh",
        SpherePoint2::fromLatitudeAndLongitude(22.356901f, 91.782967f)),
    CityStaticData(
        "Kuala Lumpur", "Malaysia",
        SpherePoint2::fromLatitudeAndLongitude(3.136966f, 101.687273f)),
    CityStaticData(
        "Medan", "Malaysia",
        SpherePoint2::fromLatitudeAndLongitude(3.595085f, 98.672622f)),
    CityStaticData(
        "Singapore", "Singapore",
        SpherePoint2::fromLatitudeAndLongitude(1.355345f, 103.867897f)),
    CityStaticData("Jakarta", "Indonesia",
                   SpherePoint2::fromLatitudeAndLongitude(-6.212489f,
                                                          106.848450f)),
    CityStaticData(
        "Surabaya", "Indonesia",
        SpherePoint2::fromLatitudeAndLongitude(-7.258463f, 112.750992f)),
    CityStaticData(
        "Bandar Seri Begawan", "Brunei",
        SpherePoint2::fromLatitudeAndLongitude(4.902375f, 114.939645f)),
    CityStaticData(
        "Sandakan", "Malaysia",
        SpherePoint2::fromLatitudeAndLongitude(5.838945f, 118.118222f)),
    CityStaticData(
        "Pontianak", "Indonesia",
        SpherePoint2::fromLatitudeAndLongitude(-0.027208f, 109.342820f)),
    CityStaticData(
        "Balikpapan", "Indonesia",
        SpherePoint2::fromLatitudeAndLongitude(-1.238310, 116.850941f)),
    CityStaticData(
        "Makassar", "Indonesia",
        SpherePoint2::fromLatitudeAndLongitude(-5.148554f, 119.432133f)),
    CityStaticData(
        "Manado", "Indonesia",
        SpherePoint2::fromLatitudeAndLongitude(1.474771f, 124.842587f)),
    CityStaticData(
        "Dili", "Timor Leste",
        SpherePoint2::fromLatitudeAndLongitude(-8.555805f, 125.560715f)),
    CityStaticData(
        "Teluk Ambon", "Indonesia",
        SpherePoint2::fromLatitudeAndLongitude(-3.656191f, 128.191125f)),
    CityStaticData(
        "Jayapura", "Indonesia",
        SpherePoint2::fromLatitudeAndLongitude(-2.595181f, 140.670967f)),
    CityStaticData(
        "Port Moresby", "Papua New Guinea",
        SpherePoint2::fromLatitudeAndLongitude(-9.445882, 147.183418f)),
    CityStaticData(
        "Davao", "Phillippines",
        SpherePoint2::fromLatitudeAndLongitude(7.190340f, 125.455124f)),
    CityStaticData(
        "Manila", "Phillippines",
        SpherePoint2::fromLatitudeAndLongitude(14.596255f, 120.981391f)),
    CityStaticData("Bangkok", "Thailand",
                   SpherePoint2::fromLatitudeAndLongitude(13.755967f,
                                                          100.504354f)),
    CityStaticData(
        "Phnom Penh", "Cambodia",
        SpherePoint2::fromLatitudeAndLongitude(11.556007f, 104.928102f)),
    CityStaticData(
        "Ho Chi Minh City", "Vietnam",
        SpherePoint2::fromLatitudeAndLongitude(10.823603f, 106.628741f)),
    CityStaticData(
        "Haiphong", "Vietnam",
        SpherePoint2::fromLatitudeAndLongitude(20.844544f, 106.687519f)),
    CityStaticData(
        "Hong Kong", "China",
        SpherePoint2::fromLatitudeAndLongitude(22.318811f, 114.169416f)),
    CityStaticData(
        "Guangzhou", "China",
        SpherePoint2::fromLatitudeAndLongitude(23.129450f, 113.262880f)),
    CityStaticData(
        "Taipei", "Taiwan",
        SpherePoint2::fromLatitudeAndLongitude(25.032999f, 121.565401f)),
    CityStaticData(
        "Shanghai", "China",
        SpherePoint2::fromLatitudeAndLongitude(31.229479f, 121.473687f)),
    CityStaticData("Qingdao", "China",
                   SpherePoint2::fromLatitudeAndLongitude(36.064810f,
                                                          120.380567f)),
    CityStaticData("Beijing", "China",
                   SpherePoint2::fromLatitudeAndLongitude(39.904081f,
                                                          116.406699f)),
    CityStaticData(
        "Dalian", "China",
        SpherePoint2::fromLatitudeAndLongitude(38.913948f, 121.613568f)),
    CityStaticData(
        "Pyongyang", "North Korea",
        SpherePoint2::fromLatitudeAndLongitude(39.039420f, 125.761985f)),
    CityStaticData(
        "Seoul", "South Korea",
        SpherePoint2::fromLatitudeAndLongitude(37.551676f, 126.991735f)),
    CityStaticData(
        "Busan", "South Korea",
        SpherePoint2::fromLatitudeAndLongitude(35.159925f, 129.048070f)),
    CityStaticData(
        "Jeju", "South Korea",
        SpherePoint2::fromLatitudeAndLongitude(33.500555f, 126.532322f)),
    CityStaticData(
        "Nagasaki", "Japan",
        SpherePoint2::fromLatitudeAndLongitude(32.753210f, 129.686259f)),
    CityStaticData("Fukuoka", "Japan",
                   SpherePoint2::fromLatitudeAndLongitude(33.570021f,
                                                          130.358030f)),
    CityStaticData("Okinawa", "Japan",
                   SpherePoint2::fromLatitudeAndLongitude(26.332695f,
                                                          127.804306f)),
    CityStaticData(
        "Hiroshima", "Japan",
        SpherePoint2::fromLatitudeAndLongitude(34.384268f, 132.455273f)),
    CityStaticData(
        "Osaka", "Japan",
        SpherePoint2::fromLatitudeAndLongitude(34.692953f, 135.501919f)),
    CityStaticData(
        "Nagoya", "Japan",
        SpherePoint2::fromLatitudeAndLongitude(35.182402f, 136.907079f)),
    CityStaticData(
        "Tokyo", "Japan",
        SpherePoint2::fromLatitudeAndLongitude(35.683422f, 139.768903f)),
    CityStaticData("Niigata", "Japan",
                   SpherePoint2::fromLatitudeAndLongitude(37.913817f,
                                                          139.034638f)),
    CityStaticData(
        "Sendai", "Japan",
        SpherePoint2::fromLatitudeAndLongitude(38.269722f, 140.868930f)),
    CityStaticData("Sapporo", "Japan",
                   SpherePoint2::fromLatitudeAndLongitude(43.061837f,
                                                          141.353347f)),
    CityStaticData(
        "Vladivostok", "Russia",
        SpherePoint2::fromLatitudeAndLongitude(43.132948f, 131.911198f)),
    CityStaticData(
        "Petropavlovsk-Kamchatskiy", "Russia",
        SpherePoint2::fromLatitudeAndLongitude(53.055506f, 158.633540f)),
    CityStaticData(
        "Noumea", "New Caledonia",
        SpherePoint2::fromLatitudeAndLongitude(-22.274874f, 166.447406f)),
    CityStaticData(
        "Port Vila", "Vanuatu",
        SpherePoint2::fromLatitudeAndLongitude(-17.734183f, 168.326385f)),
    CityStaticData("Honiara", "Solomon Islands",
                   SpherePoint2::fromLatitudeAndLongitude(-9.445695f,
                                                          159.972863f)),
    CityStaticData(
        "Auckland", "New Zealand",
        SpherePoint2::fromLatitudeAndLongitude(-36.850939f, 174.764504f)),
    CityStaticData(
        "Wellington", "New Zealand",
        SpherePoint2::fromLatitudeAndLongitude(-41.292422f, 174.778922f)),
    CityStaticData(
        "Christchurch", "New Zealand",
        SpherePoint2::fromLatitudeAndLongitude(-43.532070f, 172.630611f)),
    CityStaticData(
        "Hobart", "Australia",
        SpherePoint2::fromLatitudeAndLongitude(-42.881883f, 147.326519f)),
    CityStaticData(
        "Melbourne", "Australia",
        SpherePoint2::fromLatitudeAndLongitude(-37.813230f, 144.962805f)),
    CityStaticData(
        "Sydney", "Australia",
        SpherePoint2::fromLatitudeAndLongitude(-33.867530f, 151.208455f)),
    CityStaticData(
        "Brisbane", "Australia",
        SpherePoint2::fromLatitudeAndLongitude(-27.470993f, 153.025300f)),
    CityStaticData(
        "Perth", "Australia",
        SpherePoint2::fromLatitudeAndLongitude(-31.953552f, 115.860902f)),
    CityStaticData(
        "Adelaide", "Australia",
        SpherePoint2::fromLatitudeAndLongitude(-34.928707f, 138.600037f)),
    CityStaticData(
        "Darwin", "Australia",
        SpherePoint2::fromLatitudeAndLongitude(-12.464131f, 130.844850f)),
    CityStaticData(
        "Cairns", "Australia",
        SpherePoint2::fromLatitudeAndLongitude(-16.920509f, 145.770894f)),
    CityStaticData(
        "Suva", "Fiji",
        SpherePoint2::fromLatitudeAndLongitude(-18.140531f, 178.423302f)),
    CityStaticData(
        "Apia", "Samoa",
        SpherePoint2::fromLatitudeAndLongitude(-13.850935, -171.751257f)),
    CityStaticData(
        "Pape'ete", "French Polynesia",
        SpherePoint2::fromLatitudeAndLongitude(-17.533128f, -149.568987f)),
    CityStaticData(
        "Hanga Roa", "Chile",
        SpherePoint2::fromLatitudeAndLongitude(-27.149449f, -109.433012f)),
    CityStaticData(
        "Male", "Maldives",
        SpherePoint2::fromLatitudeAndLongitude(4.175115f, 73.509775f)),
    CityStaticData(
        "Victoria", "Seychelles",
        SpherePoint2::fromLatitudeAndLongitude(-4.620354f, 55.452298f)),
    CityStaticData(
        "Tristan da Cunha", "United Kingdom",
        SpherePoint2::fromLatitudeAndLongitude(-37.109279f, -12.279608f)),
    CityStaticData(
        "Port Blair", "India",
        SpherePoint2::fromLatitudeAndLongitude(11.623178f, 92.726038f)),
    CityStaticData(
        "Saint-Denis", "France",
        SpherePoint2::fromLatitudeAndLongitude(-20.890574f, 55.454172f)),
    CityStaticData(
        "Port Louis", "Mauritius",
        SpherePoint2::fromLatitudeAndLongitude(-20.161276f, 57.501232f)),
    CityStaticData(
        "Moroni", "Comoros",
        SpherePoint2::fromLatitudeAndLongitude(-11.717184f, 43.247438f)),
    CityStaticData(
        "Gqeberha", "South Africa",
        SpherePoint2::fromLatitudeAndLongitude(-33.961200f, 25.602150f)),
    CityStaticData(
        "Sao Tome", "Sao Tome and Principe",
        SpherePoint2::fromLatitudeAndLongitude(0.330359f, 6.733335f)),
    CityStaticData(
        "Basse-Terre", "France",
        SpherePoint2::fromLatitudeAndLongitude(15.996461f, -61.731857f)),
    CityStaticData(
        "Fort-de-France", "France",
        SpherePoint2::fromLatitudeAndLongitude(14.616100f, -61.058967f)),
    CityStaticData(
        "Port-of-Spain", "Trinidad and Tobago",
        SpherePoint2::fromLatitudeAndLongitude(10.660092f, -61.508583f)),
    CityStaticData(
        "San Andres", "Colombia",
        SpherePoint2::fromLatitudeAndLongitude(12.578058f, -81.702860f)),
    CityStaticData(
        "Providenciales", "United Kingdom",
        SpherePoint2::fromLatitudeAndLongitude(21.798958f, -72.296433f)),
    CityStaticData(
        "Bridgetown", "Barbados",
        SpherePoint2::fromLatitudeAndLongitude(13.105942f, -59.614313f)),
    CityStaticData(
        "Oranjestad", "Aruba",
        SpherePoint2::fromLatitudeAndLongitude(12.508963f, -70.010027f)),
    CityStaticData(
        "Hamilton", "United Kingdom",
        SpherePoint2::fromLatitudeAndLongitude(32.293401f, -64.786247f)),
    CityStaticData(
        "Saint-Pierre", "France",
        SpherePoint2::fromLatitudeAndLongitude(46.775417f, -56.177724f)),
    CityStaticData(
        "Midway", "USA",
        SpherePoint2::fromLatitudeAndLongitude(28.206830f, -177.381933f)),
    CityStaticData(
        "Dededo", "USA",
        SpherePoint2::fromLatitudeAndLongitude(13.543550f, 144.850673f)),
    CityStaticData(
        "Kailua-Kona", "USA",
        SpherePoint2::fromLatitudeAndLongitude(19.640993f, -155.996875f)),
    CityStaticData(
        "Honolulu", "USA",
        SpherePoint2::fromLatitudeAndLongitude(21.309919f, -157.858154f)),
    CityStaticData(
        "Lisbon", "Portugal",
        SpherePoint2::fromLatitudeAndLongitude(38.685108f, -9.238115f)),
    CityStaticData(
        "Philadelphia", "USA",
        SpherePoint2::fromLatitudeAndLongitude(39.951777f, -75.164746f)),
    CityStaticData(
        "Charleston", "USA",
        SpherePoint2::fromLatitudeAndLongitude(32.776509f, -79.930982f)),
    CityStaticData(
        "Jacksonville", "USA",
        SpherePoint2::fromLatitudeAndLongitude(30.332116f, -81.655670f)),
    CityStaticData(
        "Tampa", "USA",
        SpherePoint2::fromLatitudeAndLongitude(27.949617f, -82.457779f)),
    CityStaticData(
        "Portland", "USA",
        SpherePoint2::fromLatitudeAndLongitude(45.514489f, -122.678211f)),
    CityStaticData("Palikir", "Micronesia",
                   SpherePoint2::fromLatitudeAndLongitude(6.916774f,
                                                          158.160038f)),
    CityStaticData(
        "Arkhangel'sk", "Russia",
        SpherePoint2::fromLatitudeAndLongitude(64.545948f, 40.550692f)),
    CityStaticData(
        "Kaohsiung City", "Taiwan",
        SpherePoint2::fromLatitudeAndLongitude(22.627141f, 120.301381f)),
    CityStaticData(
        "Fuzhou", "China",
        SpherePoint2::fromLatitudeAndLongitude(26.074190f, 119.297491f)),
    CityStaticData(
        "Xiamen", "China",
        SpherePoint2::fromLatitudeAndLongitude(24.478640f, 118.087621f)),
    CityStaticData("Wenzhou", "China",
                   SpherePoint2::fromLatitudeAndLongitude(27.993281f,
                                                          120.699321f)),
    CityStaticData("Taizhou", "China",
                   SpherePoint2::fromLatitudeAndLongitude(28.657710f,
                                                          121.421077f)),
    CityStaticData(
        "Ningbo", "China",
        SpherePoint2::fromLatitudeAndLongitude(29.867671f, 121.543897f)),
};

}  // namespace earth_world
