/* Magic Book Reader
 * Copyright (C) 2015 Yan Zlobin <yan_zlobin@agniyoga.ru>
 *
 * mb-reference.c
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "mb-reference.h"

typedef struct _Genres Genres;

struct _Genres
{
	gchar *key;
	gchar *value;
};

static Genres genres_ru[] =
{
	// FictionBook 2.1
	{"sf_history", "Альтернативная история"},
	{"sf_action", "Боевая фантастика"},
	{"sf_epic", "Эпическая фантастика"},
	{"sf_heroic", "Героическая фантастика"},
	{"sf_detective", "Детективная фантастика"},
	{"sf_cyberpunk", "Киберпанк"},
	{"sf_space", "Космическая фантастика"},
	{"sf_social", "Социально-психологическая фантастика"},
	{"sf_horror", "Ужасы и Мистика"},
	{"sf_humor", "Юмористическая фантастика"},
	{"sf_fantasy", "Фэнтези"},
	{"sf", "Научная фантастика"},
	{"det_classic", "Классический детектив"},
	{"det_police", "Полицейский детектив"},
	{"det_action", "Боевик"},
	{"det_irony", "Иронический детектив"},
	{"det_history", "Исторический детектив"},
	{"det_espionage", "Шпионский детектив"},
	{"det_crime", "Криминальный детектив"},
	{"det_political", "Политический детектив"},
	{"det_maniac", "Маньяки"},
	{"det_hard", "Крутой детектив"},
	{"thriller", "Триллер"},
	{"detective", "Детектив"},
	{"prose_classic", "Классическая проза"},
	{"prose_history", "Историческая проза"},
	{"prose_contemporary", "Современная проза"},
	{"prose_counter", "Контркультура"},
	{"prose_rus_classic", "Русская классическая проза"},
	{"prose_su_classics", "Советская классическая проза"},
	{"love_contemporary", "Современные любовные романы"},
	{"love_history", "Исторические любовные романы"},
	{"love_detective", "Остросюжетные любовные романы"},
	{"love_short", "Короткие любовные романы"},
	{"love_erotica", "Эротика"},
	{"adv_western", "Вестерн"},
	{"adv_history", "Исторические приключения"},
	{"adv_indian", "Приключения про индейцев"},
	{"adv_maritime", "Морские приключения"},
	{"adv_geo", "География и путешествия"},
	{"adv_animal", "Природа и животные"},
	{"adventure", "Приключения"},
	{"child_tale", "Сказка"},
	{"child_verse", "Детские стихи"},
	{"child_prose", "Детскиая проза"},
	{"child_sf", "Детская фантастика"},
	{"child_det", "Детские остросюжетные"},
	{"child_adv", "Детские приключения"},
	{"child_education", "Детская образовательная литература"},
	{"children", "Детская литература"},
	{"poetry", "Поэзия"},
	{"dramaturgy", "Драматургия"},
	{"antique_ant", "Античная литература"},
	{"antique_european", "Европейская старинная литература"},
	{"antique_russian", "Древнерусская литература"},
	{"antique_east", "Древневосточная литература"},
	{"antique_myths", "Мифы. Легенды. Эпос"},
	{"antique", "Старинная литература"},
	{"sci_history", "История"},
	{"sci_psychology", "Психология"},
	{"sci_culture", "Культурология"},
	{"sci_religion", "Религиоведение"},
	{"sci_philosophy", "Философия"},
	{"sci_politics", "Политика"},
	{"sci_business", "Деловая литература"},
	{"sci_juris", "Юриспруденция"},
	{"sci_linguistic", "Языкознание"},
	{"sci_medicine", "Медицина"},
	{"sci_phys", "Физика"},
	{"sci_math", "Математика"},
	{"sci_chem", "Химия"},
	{"sci_biology", "Биология"},
	{"sci_tech", "Технические науки"},
	{"science", "Наука"},
	{"comp_www", "Интернет"},
	{"comp_programming", "Программирование"},
	{"comp_hard", "Компьютерное аппаратное обеспечение"},
	{"comp_soft", "Программное обеспечение"},
	{"comp_db", "Базы данных"},
	{"comp_osnet", "Операционные ситемы и сети"},
	{"computers", "Компьютеры"},
	{"ref_encyc", "Энциклопедии"},
	{"ref_dict", "Словари"},
	{"ref_ref", "Справочники"},
	{"ref_guide", "Руководства"},
	{"reference", "Справочники"},
	{"nonf_biography", "Биографии и Мемуары"},
	{"nonf_publicism", "Публицистика"},
	{"nonf_criticism", "Критика"},
	{"design", "Искусство и Дизайн"},
	{"nonfiction", "Документальная литература"},
	{"religion_rel", "Религия"},
	{"religion_esoterics", "Эзотерика"},
	{"religion_self", "Самосовершенствование"},
	{"religion", "Религия"},
	{"humor_anecdote", "Анекдоты"},
	{"humor_prose", "Юмористическая проза"},
	{"humor_verse", "Юмористические стихи"},
	{"humor", "Юмор"},
	{"home_cooking", "Кулинария"},
	{"home_pets", "Домашние животные"},
	{"home_crafts", "Хобби и ремёсла"},
	{"home_entertain", "Развлечения"},
	{"home_health", "Здоровье"},
	{"home_garden", "Сад и огород"},
	{"home_diy", "Сделай сам"},
	{"home_sport", "Спорт"},
	{"home_sex", "Эротика и секс"},
	{"home", "Домоводство"},

	// FictionBook 2.0
	{"architecture", "Искусство и Дизайн"},
	{"art", "Искусство и Дизайн"},
	{"art_instr", "Искусство и Дизайн"},
	{"artists", "Искусство и Дизайн"},
	{"fashion", "Искусство и Дизайн"},
	{"graph_design", "Искусство и Дизайн"},
	{"performance", "Драматургия"},
	{"photography", "Искусство и Дизайн"},
	{"people", "Биографии и Мемуары"},
	{"biography", "Биографии и Мемуары"},
	{"biogr_arts", "Биографии и Мемуары"},
	{"biogr_ethnic", "Биографии и Мемуары"},
	{"biogr_family", "Биографии и Мемуары"},
	{"biogr_historical", "Биографии и Мемуары"},
	{"biogr_leaders", "Биографии и Мемуары"},
	{"biogr_professionals", "Биографии и Мемуары"},
	{"biogr_sports", "Биографии и Мемуары"},
	{"biogr_travel", "Биографии и Мемуары"},
	{"business", "Деловая литература"},
	{"biz_accounting", "Деловая литература"},
	{"biz_beogr", "Биографии и Мемуары"},
	{"biz_life", "Деловая литература"},
	{"biz_careers", "Деловая литература"},
	{"biz_economics", "Деловая литература"},
	{"biz_finance", "Деловая литература"},
	{"biz_international", "Деловая литература"},
	{"biz_professions", "Деловая литература"},
	{"biz_investing", "Деловая литература"},
	{"biz_management", "Деловая литература"},
	{"biz_sales", "Деловая литература"},
	{"biz_personal_fin", "Деловая литература"},
	{"biz_ref", "Деловая литература"},
	{"biz_small_biz", "Деловая литература"},
	{"child_3", "Сказка"},
	{"child_4", "Детскиая проза"},
	{"child_9", "Детскиая проза"},
	{"child_animals", "Природа и животные"},
	{"child_art", "Детская литература"},
	{"child_computers", "Компьютеры"},
	{"child_edu", "Детская образовательная литература"},
	{"child_history", "Детскиая проза"},
	{"child_obsessions", "Детская литература"},
	{"child_people", "Детская литература"},
	{"child_characters", "Детскиая проза"},
	{"child_ref", "Детская литература"},
	{"child_religion", "Религия"},
	{"child_nature", "Детская образовательная литература"},
	{"child_series", "Детская литература"},
	{"child_sports", "Детская литература"},
	{"chris_bibles", "Религия"},
	{"chris_pravoslavie", "Религия"},
	{"chris_catholicism", "Религия"},
	{"chris_living", "Религия"},
	{"chris_history", "Религия"},
	{"chris_edu", "Религия"},
	{"chris_evangelism", "Религия"},
	{"chris_fiction", "Религия"},
	{"chris_holidays", "Детскиая проза"},
	{"chris_jesus", "Религия"},
	{"chris_mormonism", "Религия"},
	{"chris_orthodoxy", "Религия"},
	{"chris_protestantism", "Религия"},
	{"chris_ref", "Религия"},
	{"chris_theology", "Религия"},
	{"chris_devotion", "Религия"},
	{"compusers", "Компьютеры"},
	{"comp_office", "Компьютеры"},
	{"comp_cert", "Компьютеры"},
	{"comp_games", "Компьютеры"},
	{"comp_sci", "Компьютеры"},
	{"comp_biz", "Компьютеры"},
	{"comp_graph", "Компьютеры"},
	{"comp_hardware", "Компьютерное аппаратное обеспечение"},
	{"comp_microsoft", "Компьютеры"},
	{"comp_networking", "Операционные ситемы и сети"},
	{"comp_os", "Операционные ситемы и сети"},
	{"comp_software", "Программное обеспечение"},
	{"cooking", "Кулинария"},
	{"cook_baking", "Кулинария"},
	{"cook_can", "Кулинария"},
	{"cook_art", "Кулинария"},
	{"cook_drink", "Кулинария"},
	{"cook_gastronomy", "Кулинария"},
	{"cook_meals", "Кулинария"},
	{"cook_natural", "Кулинария"},
	{"cook_outdoor", "Кулинария"},
	{"cook_pro", "Кулинария"},
	{"cook_quick", "Кулинария"},
	{"cook_ref", "Кулинария"},
	{"cook_regional", "Кулинария"},
	{"cook_appliances", "Кулинария"},
	{"cook_diet", "Кулинария"},
	{"cook_spec", "Кулинария"},
	{"cook_veget", "Кулинария"},
	{"entertainment", "Развлечения"},
	{"entert_comics", "Развлечения"},
	{"entert_games", "Развлечения"},
	{"entert_humor", "Развлечения"},
	{"entert_movies", "Развлечения"},
	{"entert_music", "Развлечения"},
	{"nonfiction_pop_culture", "Развлечения"},
	{"entert_radio", "Развлечения"},
	{"entert_tv", "Развлечения"},
	{"gay_biogr", "Биографии и Мемуары"},
	{"history_gay", "Биографии и Мемуары"},
	{"gay_mystery", "Ужасы и Мистика"},
	{"gay_nonfiction", "Документальная литература"},
	{"gay_parenting", "Домоводство"},
	{"gay_travel", "География и путешествия"},
	{"health", "Здоровье"},
	{"health_aging", "Медицина"},
	{"health_alt_medicine", "Медицина"},
	{"health_beauty", "Здоровье"},
	{"health_cancer", "Медицина"},
	{"health_death", "Здоровье"},
	{"health_diets", "Здоровье"},
	{"health_dideases", "Здоровье"},
	{"health_fitness", "Здоровье"},
	{"health_men", "Здоровье"},
	{"health_mental", "Психология"},
	{"health_nutrition", "Здоровье"},
	{"health_personal", "Здоровье"},
	{"health_psy", "Психология"},
	{"health_recovery", "Здоровье"},
	{"health_ref", "Здоровье"},
	{"health_rel", "Здоровье"},
	{"health_first_aid", "Здоровье"},
	{"health_self_help", "Здоровье"},
	{"health_sex", "Эротика и секс"},
	{"health_women", "Здоровье"},
	{"history_africa", "История"},
	{"history_america", "История"},
	{"history_ancient", "История"},
	{"history_asia", "История"},
	{"history_australia", "История"},
	{"history_europe", "История"},
	{"history_study", "История"},
	{"history_jewish", "История"},
	{"history_middle_east", "История"},
	{"histor_military", "История"},
	{"history_military_science", "История"},
	{"history_russia", "История"},
	{"history_usa", "История"},
	{"history_world", "История"},
	{"home_collect", "Хобби и ремёсла"},
	{"Хобби и ремёсла", "Хобби и ремёсла"},
	{"Развлечения", "Развлечения"},
	{"home_expert", "Сделай сам"},
	{"home_design", "Сделай сам"},
	{"home_howto", "Сделай сам"},
	{"home_interior_design", "Сделай сам"},
	{"home_cottage", "Домоводство"},
	{"home_weddings", "Домоводство"},
	{"horror", "Ужасы и Мистика"},
	{"horror_antology", "Ужасы и Мистика"},
	{"horror_british", "Ужасы и Мистика"},
	{"horror_fantasy", "Ужасы и Мистика"},
	{"horror_erotic", "Ужасы и Мистика"},
	{"horror_ghosts", "Ужасы и Мистика"},
	{"horror_graphic", "Ужасы и Мистика"},
	{"horror_occult", "Ужасы и Мистика"},
	{"horror_ref", "Ужасы и Мистика"},
	{"horror_usa", "Ужасы и Мистика"},
	{"horror_vampires", "Ужасы и Мистика"},
	{"literature", "Классическая проза"},
	{"literature_books", "Классическая проза"},
	{"literature_british", "Классическая проза"},
	{"literature_classics", "Классическая проза"},
	{"literature_drama", "Классическая проза"},
	{"literature_essay", "Классическая проза"},
	{"literature_adv", "Приключения"},
	{"literature_antology", "Классическая проза"},
	{"literature_erotica", "Эротика"},
	{"literature_fairy", "Сказка"},
	{"literature_saga", "Классическая проза"},
	{"literature_gay", "Контркультура"},
	{"literature_history", "Историческая проза"},
	{"literature_men_advent", "Приключения"},
	{"literature_political", "Современная проза"},
	{"literature_religion", "Религия"},
	{"literature_sea", "Морские приключения"},
	{"literature_sports", "Спорт"},
	{"literature_war", "Современная проза"},
	{"literature_western", "Вестерн"},
	{"literature_critic", "Историческая проза"},
	{"literature_letters", "Биографии и Мемуары"},
	{"literature_poetry", "Поэзия"},
	{"literature_short", "Классическая проза"},
	{"literature_usa", "Классическая проза"},
	{"literature_women", "Современные любовные романы"},
	{"literature_world", "Классическая проза"},
	{"literature_rus_classsic", "Русская классическая проза"},
	{"literature_su_classics", "Советская классическая проза"},
	{"mystery", "Исторический детектив"},
	{"thriller_mystery", "Детектив"},
	{"thriller_police", "Полицейский детектив"},
	{"nonfiction_avto", "Документальная литература"},
	{"nonfiction_crime", "Документальная литература"},
	{"nonfiction_events", "Документальная литература"},
	{"nonfiction_edu", "Наука"},
	{"nonfiction_gov", "Наука"},
	{"nonfiction_holidays", "Наука"},
	{"nonfiction_law", "Юриспруденция"},
	{"nonfiction_philosophy", "Философия"},
	{"nonfiction_politics", "Документальная литература"},
	{"nonfiction_social_sci", "Наука"},
	{"nonfiction_antropology", "История"},
	{"science_archaeology", "История"},
	{"nonfiction_traditions", "Документальная литература"},
	{"nonfiction_demography", "Документальная литература"},
	{"nonfiction_racism", "Документальная литература"},
	{"nonfiction_emigration", "Документальная литература"},
	{"nonfiction_ethnology", "Наука"},
	{"nonfiction_folklor", "Мифы. Легенды. Эпос"},
	{"nonfiction_gender", "Наука"},
	{"nonfiction_gerontology", "Наука"},
	{"nonfiction_hum_geogr", "Наука"},
	{"nonfiction_methodology", "Наука"},
	{"nonfiction_philantropy", "Документальная литература"},
	{"nonfiction_pornography", "Эротика и секс"},
	{"nonfiction_ref", "Справочники"},
	{"nonfiction_research", "Наука"},
	{"nonfiction_social_work", "Наука"},
	{"nonfiction_sociology", "Наука"},
	{"nonfiction_spec_group", "Наука"},
	{"nonfiction_stat", "Наука"},
	{"nonfiction_transportation", "Документальная литература"},
	{"nonfiction_true_accounts", "Документальная литература"},
	{"nonfiction_urban", "Документальная литература"},
	{"nonfiction_women", "Документальная литература"},
	{"outdoors_birdwatching", "Биология"},
	{"outdoors_conservation", "Документальная литература"},
	{"outdoors_ecology", "Биология"},
	{"outdoors_ecosystems", "Биология"},
	{"outdoors_env", "Биология"},
	{"outdoors_fauna", "Биология"},
	{"outdoors_field_guides", "Руководства"},
	{"outdoors_flora", "Биология"},
	{"outdoors_hiking", "Хобби и ремёсла"},
	{"outdoors_hunt_fish", "Хобби и ремёсла"},
	{"outdoors_resources", "Наука"},
	{"outdoors_nature_writing", "Биология"},
	{"outdoors_outdoor_recreation", "Спорт"},
	{"outdoors_ref", "Биология"},
	{"outdoors_survive", "Спорт"},
	{"outdoors_travel", "География и путешествия"},
	{"family", "Домоводство"},
	{"family_adoption", "Домоводство"},
	{"family_aging_parents", "Домоводство"},
	{"family_edu", "Домоводство"},
	{"family_activities", "Домоводство"},
	{"family_health", "Здоровье"},
	{"family_relations", "Домоводство"},
	{"family_fertility", "Здоровье"},
	{"family_humor", "Юмор"},
	{"family_lit_guide", "Домоводство"},
	{"family_parenting", "Здоровье"},
	{"family_pregnancy", "Здоровье"},
	{"family_ref", "Справочники"},
	{"family_special_needs", "Здоровье"},
	{"professional_finance", "Деловая литература"},
	{"professional_management", "Деловая литература"},
	{"professional_edu", "Наука"},
	{"professional_enginering", "Технические науки"},
	{"professional_law", "Юриспруденция"},
	{"professional_medical", "Медицина"},
	{"professional_sci", "Технические науки"},
	{"references", "Справочники"},
	{"ref_almanacs", "Справочники"},
	{"ref_careers", "Справочники"},
	{"ref_catalogs", "Справочники"},
	{"ref_cons_guides", "Справочники"},
	{"ref_edu", "Деловая литература"},
	{"ref_encyclopedia", "Энциклопедии"},
	{"ref_etiquette", "Справочники"},
	{"ref_langs", "Справочники"},
	{"ref_fun", "Справочники"},
	{"ref_genealogy", "История"},
	{"ref_books", "Справочники"},
	{"ref_quotations", "Справочники"},
	{"ref_study_guides", "Справочники"},
	{"ref_words", "Справочники"},
	{"ref_writing", "Современная проза"},
	{"religion_bibles", "Религия"},
	{"religion_buddhism", "Религия"},
	{"christianity", "Религия"},
	{"religion_earth", "Религия"},
	{"religion_fiction", "Религия"},
	{"religion_hinduism", "Религия"},
	{"religion_islam", "Религия"},
	{"religion_judaism", "Религия"},
	{"religion_new_age", "Религия"},
	{"religion_occult", "Эзотерика"},
	{"religion_religious_studies", "Религия"},
	{"religion_spirituality", "Эзотерика"},
	{"religion_east", "Самосовершенствование"},
	{"religion_other", "Религия"},
	{"romance", "Классическая проза"},
	{"romance_anthologies", "Классическая проза"},
	{"romance_contemporary", "Современная проза"},
	{"romance_fantasy", "Фэнтези"},
	{"romance_sf", "Научная фантастика"},
	{"romance_historical", "Историческая проза"},
	{"romance_multicultural", "Современная проза"},
	{"romance_regency", "Классическая проза"},
	{"romance_religion", "Религия"},
	{"romance_romantic_suspense", "Современные любовные романы"},
	{"romance_series", "Современная проза"},
	{"romance_time_travel", "Научная фантастика"},
	{"science_agri", "Наука"},
	{"science_astronomy", "Наука"},
	{"science_behavioral sciences", "Психология"},
	{"science_psy", "Психология"},
	{"science_biolog", "Биология"},
	{"science_chemistry", "Химия"},
	{"science_earth", "Наука"},
	{"science_edu", "Наука"},
	{"science_evolution", "Наука"},
	{"science_measurement", "Наука"},
	{"science_history_philosophy", "История"},
	{"science_math", "Математика"},
	{"science_medicine", "Медицина"},
	{"science_eco", "Наука"},
	{"science_physics", "Физика"},
	{"science_ref", "Наука"},
	{"science_technology", "Технические науки"},
	{"gaming", "Научная фантастика"},
	{"sf_writing", "Научная фантастика"},
	{"fantasy_alt_hist", "Альтернативная история"},
	{"sf_cyber_punk", "Киберпанк"},
	{"sport", "Спорт"},
	{"teens_beogr", "Биографии и Мемуары"},
	{"teens_health", "Спорт"},
	{"teens_history", "Исторические приключения"},
	{"teens_horror", "Ужасы и Мистика"},
	{"teens_literature", "Классическая проза"},
	{"teens_mysteries", "Детектив"},
	{"teens_ref", "Справочники"},
	{"teens_religion", "Религия"},
	{"teens_school_sports", "Домоводство"},
	{"teens_sf", "Научная фантастика"},
	{"teens_tech", "Наука"},
	{"teens_series", "Детские приключения"},
	{"teens_social", "Психология"},
	{"travel", "География и путешествия"},
	{"travel_africa", "География и путешествия"},
	{"travel_asia", "География и путешествия"},
	{"travel_australia", "География и путешествия"},
	{"travel_canada", "География и путешествия"},
	{"travel_caribbean", "География и путешествия"},
	{"travel_europe", "География и путешествия"},
	{"travel_guidebook_series", "География и путешествия"},
	{"travel_lat_am", "География и путешествия"},
	{"travel_middle_east", "География и путешествия"},
	{"travel_polar", "География и путешествия"},
	{"travel_spec", "География и путешествия"},
	{"travel_usa", "География и путешествия"},
	{"travel_rus", "География и путешествия"},
	{"travel_ex_ussr", "География и путешествия"},
	{"women_divorce", "Домоводство"},
	{"women_domestic", "Домоводство"},
	{"women_child", "Домоводство"},
	{"women_single", "Домоводство"}
};

#define MB_REFERENCE_GET_PRIVATE(obj) (G_TYPE_INSTANCE_GET_PRIVATE ((obj), \
	MB_TYPE_REFERENCE, MbReferencePrivate))

struct _MbReferencePrivate
{
	GHashTable *genres;
};

static void mb_reference_finalize (GObject *object);

static void genres_init (MbReference *reference);

G_DEFINE_TYPE (MbReference, mb_reference, G_TYPE_OBJECT);

static void
mb_reference_init (MbReference *reference)
{
	MbReferencePrivate *priv;

	priv = MB_REFERENCE_GET_PRIVATE (reference);
	reference->priv = priv;

	priv->genres = g_hash_table_new_full (g_str_hash, g_str_equal, g_free,
	                                      g_free);

	genres_init (reference);
}

static void
mb_reference_class_init (MbReferenceClass *klass)
{
	GObjectClass *gobject_class;

	g_type_class_add_private (klass, sizeof (MbReferencePrivate));

	gobject_class = G_OBJECT_CLASS (klass);
	
	gobject_class->finalize = mb_reference_finalize;
}

static void
mb_reference_finalize (GObject *object)
{
	MbReference *reference;
	MbReferencePrivate *priv;

	reference = MB_REFERENCE (object);
	priv = reference->priv;
	
	g_hash_table_destroy (priv->genres);
	g_hash_table_unref (priv->genres);

	G_OBJECT_CLASS (mb_reference_parent_class)->finalize (object);
}

MbReference *
mb_reference_new (void)
{
	MbReference *reference;

	reference = g_object_new (MB_TYPE_REFERENCE, NULL);

	return reference;
}

gchar *
mb_reference_get_genre_name (MbReference *reference, gchar *key)
{
	return g_hash_table_lookup (reference->priv->genres, key);
}

gchar *
mb_reference_get_genre_name_from_list (MbReference *reference, GSList *key_list)
{
	GSList *list;
	GString *genre_list;
	gchar *value;

	genre_list = g_string_new ("");

	for (list = key_list; list; list = list->next)
	{
		value = mb_reference_get_genre_name (reference, list->data);

		if (value)
		{
			if (genre_list->len > 0)
			{
				g_string_append (genre_list, ", ");
			}
			
			g_string_append (genre_list, value);
		}
	}

	return g_string_free (genre_list, FALSE);
}

GList *
mb_reference_get_genre_list (MbReference *reference)
{
	return g_hash_table_get_keys (reference->priv->genres);
}

static void
genres_init (MbReference *reference)
{
	MbReferencePrivate *priv;
	gint genres_size;
	gint i;

	priv = reference->priv;
	genres_size = sizeof (genres_ru) / sizeof (Genres);

	for (i = 0; i < genres_size; i++)
	{
		g_hash_table_insert (priv->genres, (gpointer) genres_ru[i].key,
		                     genres_ru[i].value);
	}
}

