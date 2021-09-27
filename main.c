#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <gtk/gtk.h>

typedef struct 
{
	GtkWidget *window;
	GtkWidget *edit, *edit_n, *text;
	GtkWidget *arab, *rim;
	GtkWidget *seph1, *seph2, *sepv1;
	GtkWidget *list;
	GtkWidget *vbox, *hbox;
	GtkWidget *label;
	GtkWidget *button, *button_n;
}MainForm;

typedef struct
{
	int sm[64];
	char str[512];
	int count;
	char str_arab[16];
	char str_error[128];
}string_numbers_t;

typedef struct
{
	int sm_0_9[10];
	int sm_10_19[10];
	int sm_20_90[8];
	int sm_hundred;
	int sm_thousand;
	int sm_million;
	int sm_and;
	char *str;
	int count;
}default_string_numbers_t;


default_string_numbers_t dn;

static void button_click(GtkWidget* button, MainForm *form);
static void button_n_click(GtkWidget* button, MainForm *form);
static void destroy(GtkWidget* window, gpointer data);
static int english_to_arab(string_numbers_t *sn);
static int arab_to_rim(int arab, char *str_n);
static int arab_to_english(int arab, char *str);
static int arab_0_99_to_english(int arab, char *str);
static int arab_0_999_to_english( int arab, char *str);
static void set_all_numbers();
static void del_space(char *str, string_numbers_t *sn);
int roman(int i, int j, char *c, char *str_n);

int main(int argc, char *argv[])
{
	MainForm form;

	// занесение названий чисел требуемых для составления
	set_all_numbers();

	// Инициализируем модуль GTK
	gtk_init(&argc, &argv);

	// Создаем основное окно
	form.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(form.window), "TAiFYa Programm #1");
	gtk_container_set_border_width(GTK_CONTAINER(form.window), 10);
	gtk_window_set_position(GTK_WINDOW(form.window),GTK_WIN_POS_CENTER);
	gtk_window_set_resizable(GTK_WINDOW(form.window), FALSE);
	g_signal_connect(G_OBJECT(form.window), "destroy", G_CALLBACK(destroy), NULL);

	// Создание основного контейнера (вертикальный бокс)
	form.vbox = gtk_vbox_new(0, 10);
	gtk_container_add(GTK_CONTAINER(form.window), form.vbox);

	form.hbox = gtk_hbox_new(0, 10);
	gtk_container_add(GTK_CONTAINER(form.vbox), form.hbox);

	// надпись
	form.label = gtk_label_new ("Ведите число от 0 до 999 999 999:");
	gtk_misc_set_alignment (GTK_MISC (form.label), 0, 0);
	gtk_box_pack_start (GTK_BOX(form.hbox), form.label, FALSE, FALSE, 0);
	gtk_widget_show (form.label);

	// поле ввода
	form.edit_n = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(form.hbox), form.edit_n, FALSE, FALSE, 0);
	gtk_widget_show(form.edit_n);

	// кнопка
	form.button_n = gtk_button_new_with_label("Преоброзавать");
	gtk_box_pack_start(GTK_BOX(form.hbox), form.button_n, FALSE, FALSE, 0);
	gtk_widget_show(form.button_n);

	g_signal_connect(G_OBJECT(form.button_n), "clicked", G_CALLBACK(button_n_click), &form);

	gtk_widget_show (form.hbox);

	// надпись
	form.label = gtk_label_new ("Ведите число на английском от 0 до 999 999 999 прописью:");
	gtk_misc_set_alignment (GTK_MISC (form.label), 0, 0);
	gtk_box_pack_start (GTK_BOX(form.vbox), form.label, FALSE, FALSE, 0);
	gtk_widget_show (form.label);

	// поле ввода
	form.edit = gtk_entry_new();
	gtk_box_pack_start(GTK_BOX(form.vbox), form.edit, FALSE, FALSE, 0);
	gtk_widget_show(form.edit);

	// кнопка
	form.button = gtk_button_new_with_label("Конвертировать");
	gtk_box_pack_start(GTK_BOX(form.vbox), form.button, FALSE, FALSE, 0);
	gtk_widget_show(form.button);

	// разделитель
	form.seph1 = gtk_hseparator_new ();
	gtk_box_pack_start(GTK_BOX (form.vbox), form.seph1, FALSE, TRUE, 0);
	gtk_widget_show (form.seph1);

	// надпись
	form.label = gtk_label_new ("Число арабскими цифрами:");
	gtk_misc_set_alignment (GTK_MISC (form.label), 0, 0);
	gtk_box_pack_start (GTK_BOX(form.vbox), form.label, FALSE, FALSE, 0);
	gtk_widget_show (form.label);

	// вывод арабских цифр
	form.arab = gtk_entry_new();
	g_object_set(G_OBJECT(form.arab), "editable", FALSE, NULL);
	gtk_box_pack_start(GTK_BOX(form.vbox), form.arab, FALSE, FALSE, 0);
	gtk_widget_show(form.arab);

	// надпись
	form.label = gtk_label_new ("Число римскими цифрами:");
	gtk_misc_set_alignment (GTK_MISC (form.label), 0, 0);
	gtk_box_pack_start (GTK_BOX(form.vbox), form.label, FALSE, FALSE, 0);
	gtk_widget_show (form.label);

	// вывод римских цифр
	form.rim = gtk_entry_new();
	g_object_set(G_OBJECT(form.rim), "editable", FALSE, NULL);
	gtk_box_pack_start(GTK_BOX(form.vbox), form.rim, FALSE, FALSE, 0);
	gtk_widget_show(form.rim);

	// разделитель
	form.seph2 = gtk_hseparator_new ();
	gtk_box_pack_start(GTK_BOX (form.vbox), form.seph2, FALSE, TRUE, 0);
	gtk_widget_show (form.seph2);

	// надпись
	form.label = gtk_label_new ("Текст ошибки:");
	gtk_misc_set_alignment (GTK_MISC (form.label), 0, 0);
	gtk_box_pack_start (GTK_BOX(form.vbox), form.label, FALSE, FALSE, 0);
	gtk_widget_show (form.label);

	// вывод текста ошибки
	form.text = gtk_entry_new();
	g_object_set(G_OBJECT(form.text), "editable", FALSE, NULL);
	gtk_box_pack_start(GTK_BOX(form.vbox), form.text, FALSE, FALSE, 0);
	gtk_widget_show(form.text);

	g_signal_connect(G_OBJECT(form.button), "clicked", G_CALLBACK(button_click), &form);

	// Показываем главное окно
	gtk_widget_show(form.vbox);
	gtk_widget_show(form.window);
	gtk_main();


	return 0;
}


static void destroy( GtkWidget* window, gpointer data )
{
	gtk_main_quit();
}

static void button_click(GtkWidget* button, MainForm *form)
{
	string_numbers_t sn;
	char *str;
	char str_n[512];

	str = (char*)gtk_entry_get_text(GTK_ENTRY(form->edit));
	gtk_entry_set_text(GTK_ENTRY(form->text), (gchar*)"");
	gtk_entry_set_text(GTK_ENTRY(form->arab), (gchar*)"");
	gtk_entry_set_text(GTK_ENTRY(form->rim), (gchar*)"");

	del_space(str, &sn);
	
	if (sn.count < 1)
	{
		gtk_entry_set_text(GTK_ENTRY(form->text), (gchar*)"Введена пустая строка");
		return;
	}
	
	if (english_to_arab(&sn))
		gtk_entry_set_text(GTK_ENTRY(form->text), (gchar*)sn.str_error);
	else
		gtk_entry_set_text(GTK_ENTRY(form->arab), (gchar*)sn.str_arab);

	arab_to_rim(atoi(sn.str_arab), str_n);
	gtk_entry_set_text(GTK_ENTRY(form->rim), (gchar*)str_n);

	return;
}

static void button_n_click(GtkWidget* button, MainForm *form)
{
	char *str;
	int i;
	char str_n[512];

	str = (char*)gtk_entry_get_text(GTK_ENTRY(form->edit_n));
	gtk_entry_set_text(GTK_ENTRY(form->text), (gchar*)"");
	gtk_entry_set_text(GTK_ENTRY(form->arab), (gchar*)"");
	gtk_entry_set_text(GTK_ENTRY(form->rim), (gchar*)"");
	gtk_entry_set_text(GTK_ENTRY(form->edit), (gchar*)"");

	if (strlen(str) > 9)
	{
		gtk_entry_set_text(GTK_ENTRY(form->text), (gchar*)"Число превышает 999 999 999");
		return;
	}

	for (i = 0; i < strlen(str); i++)
	{
		if (!isdigit(str[i]) && !isspace(str[i]))
		{
			gtk_entry_set_text(GTK_ENTRY(form->text), (gchar*)"Неверно набрано число");
			return;
		}
	}

	arab_to_english(atoi(str), str_n);
	gtk_entry_set_text(GTK_ENTRY(form->edit), (gchar*)str_n);

}

static void set_all_numbers()
{
	int i;
	int len;

	dn.str = 	"one\0two\0three\0four\0five\0six\0seven\0eight\0nine\0zero\0"
				"ten\0eleven\0twelve\0thirteen\0fourteen\0fifteen\0sixteen\0seventeen\0eighteen\0nineteen\0"
				"twenty\0thirty\0forty\0fifty\0sixty\0seventy\0eighty\0ninety\0"
				"hundred\0thousand\0million\0and\0";

	len = 0;
	for (i = 0; i < 10; i++)
	{
		dn.sm_0_9[i] = len;
		len += strlen(dn.str+len) + 1;
	}
	for (i = 0; i < 10; i++)
	{
		dn.sm_10_19[i] = len;
		len += strlen(dn.str+len) + 1;
	}
	for (i = 0; i < 8; i++)
	{
		dn.sm_20_90[i] = len;
		len += strlen(dn.str+len) + 1;
	}
	dn.sm_hundred = len;
	len += strlen(dn.str+len) + 1;
	dn.sm_thousand = len;
	len += strlen(dn.str+len) + 1;
	dn.sm_million = len;
	len += strlen(dn.str+len) + 1;
	dn.sm_and = len;
}

static void del_space(char *str, string_numbers_t *sn)
{
	int i;
	int sm;
	enum
	{
		ST_WORD,
		ST_NONE
	}st;

	sn->count = 0;
	memset(sn->str, 0, 512);
	sm = 0;
	sn->sm[sn->count++] = sm;
	st = ST_NONE;
	for (i = 0; i < strlen(str); i++)
	{
		switch (st)
		{
			case ST_WORD:
				if (isspace(str[i]))
				{
					sn->sm[sn->count++] = ++sm;
					st = ST_NONE;
				}
				else if (str[i] == '-')
				{
					sn->sm[sn->count++] = ++sm;
					sn->str[sm++] = str[i];
					sn->sm[sn->count++] = ++sm;
					st = ST_NONE;
				}
				else
				{
					sn->str[sm++] = str[i];
					if (isupper(sn->str[sm-1]))
						sn->str[sm-1] = tolower(sn->str[sm-1]);
				}
				break;
			case ST_NONE:
				if (!isspace(str[i]) && (str[i] != '-'))
				{
					i--;
					st = ST_WORD;
					break;
				}
				if (str[i] == '-')
				{
					sn->str[sm++] = str[i];
					sn->sm[sn->count++] = ++sm;
				}
				break;
		}
	}
	
	if (st == ST_NONE)
		sn->count--;
}

static int arab_to_rim(int arab, char *str_n)
{
	if ((arab > 30000) || (arab < 1))
	{
		sprintf(str_n, "%s", "Число не входит в диапазон от 1 до 30000");
		return 0;
	}
	memset(str_n, 0, 512);

	arab=roman(arab, 10000, "ↂ", str_n);
	arab=roman(arab, 9000, "Mↁ", str_n);
	arab=roman(arab, 5000, "ↁ", str_n);
	arab=roman(arab, 4000, "Mↁ", str_n);
	arab=roman(arab, 1000, "M", str_n);
    arab=roman(arab, 900, "CM", str_n);
	arab=roman(arab, 500, "D", str_n);
	arab=roman(arab, 400, "CD", str_n);
	arab=roman(arab, 100, "C", str_n);
	arab=roman(arab, 90, "XC", str_n);
	arab=roman(arab, 50, "L", str_n);
	arab=roman(arab, 40, "XL", str_n);
	arab=roman(arab, 10, "X", str_n);
	arab=roman(arab, 9, "IX", str_n);
	arab=roman(arab, 5, "V", str_n);
	arab=roman(arab, 4, "IV", str_n);
	arab=roman(arab, 1, "I", str_n);

	return 0;
}

int roman(int i, int j, char *c, char *str_n)
{
	while (i >= j)
	{
		strcat(str_n, c);
		i -= j;
	}
	return(i);
}

static int english_to_arab(string_numbers_t *sn)
{
	int i;
	int arab;
	int hundred=0;
	int thousand=0;
	int million=0;
	int err;
	enum
	{
		ST_HUNDRED = 0,
		ST_NUM_20_90,
		ST_NUM_0_9,
		ST_NONE,
		ST_ANY
	}st;

	arab = 0;
	st = ST_HUNDRED;
	err = 0;
	for (i = 0; i < sn->count; i++)
	{
		if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[0]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 1;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[1]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 2;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[2]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 3;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[3]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 4;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[4]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 5;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[5]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 6;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[6]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 7;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[7]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 8;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[8]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 9;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_0_9[9]))
		{
			if (st == ST_NONE)
				err = 1;
			arab += 0;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[0]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 10;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[1]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 11;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[2]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 12;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[3]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 13;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[4]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 14;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[5]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 15;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[6]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 16;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[7]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 17;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[8]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 18;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_10_19[9]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 19;
			st = ST_NONE;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_20_90[0]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 20;
			st = ST_NUM_20_90;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_20_90[1]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 30;
			st = ST_NUM_20_90;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_20_90[2]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 40;
			st = ST_NUM_20_90;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_20_90[3]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 50;
			st = ST_NUM_20_90;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_20_90[4]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 60;
			st = ST_NUM_20_90;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_20_90[5]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 70;
			st = ST_NUM_20_90;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_20_90[6]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_0_9))
				err = 1;
			arab += 80;
			st = ST_NUM_0_9;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_20_90[7]))
		{
			if ((st == ST_NONE) || (st == ST_NUM_20_90))
				err = 1;
			arab += 90;
			st = ST_NUM_20_90;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_hundred))
		{
			if (arab == 0)
				err = 2;
			arab *= 100;
			hundred = arab;
			arab = 0;
			st = ST_ANY;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_thousand))
		{
			if (arab == 0)
				err = 2;
			arab += hundred;
			arab *= 1000;
			thousand = arab;
			arab = 0;
			hundred = 0;
			st = ST_ANY;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_million))
		{
			if (arab == 0)
				err = 2;
			arab += hundred;
			arab *= 1000000;
			million = arab;
			arab = 0;
			hundred = 0;
			st = ST_ANY;
		}
		else if (!strcmp(sn->str + sn->sm[i], dn.str + dn.sm_and))
		{
			if (st != ST_ANY)
				err = 4;
		}
		else if (!strcmp(sn->str + sn->sm[i], "-"))
		{
			if (st != ST_NUM_20_90)
				err = 3;
		}
		else
		{
			sprintf(sn->str_error,"Обнаружено неизвестное слово '%s'(%d)", sn->str + sn->sm[i], i + 1);
			return 1;
		}

		switch(err)
		{
			case 0:
				break;
			case 1:
			{
				sprintf(sn->str_error, "Неверный порядок слов. '%s'(%d) не может идти после '%s'(%d)",
					sn->str + sn->sm[i], i+1, sn->str + sn->sm[i-1], i);
				return 1;
			}
			case 2:
			{
				sprintf(sn->str_error, "Неверный порядок слов. Перед словом '%s'(%d) должно быть написано число",
					sn->str + sn->sm[i], i+1);
				return 1;
			}
			case 3:
			{
				sprintf(sn->str_error, "Между словами '%s'(%d) и '%s'(%d) дефис не требуется.",
					sn->str + sn->sm[i-1], i, sn->str + sn->sm[i+1], i+2);
				return 1;
			}
			case 4:
			{
				sprintf(sn->str_error, "Между словами '%s'(%d) и '%s'(%d) 'and' не требуется.",
					sn->str + sn->sm[i-1], i, sn->str + sn->sm[i+1], i+2);
				return 1;
			}
		}
	}
	arab += million + thousand + hundred;

	sprintf(sn->str_arab, "%d", arab);

	return 0;
}

static int arab_to_english(int arab, char *str)
{
	int million;
	int thousand;
	int tmp;
	char str_tmp[256];

	memset(str, 0, 512);

	tmp = arab;
	million = tmp / 1000000;
	tmp -= million * 1000000;
	thousand = tmp / 1000;
	tmp -= thousand *1000;

	if ((million == 0) && (thousand == 0))
	{
		arab_0_999_to_english(tmp, str_tmp);
		sprintf(str, "%s", str_tmp);
	}
	else
	{
		if (million == 0)
		{
			arab_0_999_to_english(thousand, str_tmp);
			if (tmp == 0)
				sprintf(str, "%s thousand", str_tmp);
			else if ((int) tmp / 100 == 0)
				sprintf(str, "%s thousand and ", str_tmp);
			else
				sprintf(str, "%s thousand ", str_tmp);
			if (tmp != 0)
			{
				arab_0_999_to_english(tmp, str_tmp);
				strcat(str, str_tmp);
			}
		}
		else
		{
			arab_0_999_to_english(million, str_tmp);
			sprintf(str, "%s million ", str_tmp);
			if (thousand != 0)
			{
				arab_0_999_to_english(thousand, str_tmp);
				strcat(str, str_tmp);
				if (tmp == 0)
				strcat(str, " thousand");
					else if ((int) tmp / 100 == 0)
				strcat(str, " thousand and ");
					else
				strcat(str, " thousand ");
			}
			if (tmp != 0)
			{
				arab_0_999_to_english(tmp, str_tmp);
				strcat(str, str_tmp);
			}
		}
	}

	return 0;
}

static int arab_0_999_to_english( int arab, char *str)
{
	char str_tmp[128];
	int hundred;

	hundred = arab / 100;
	arab -= hundred * 100;

	if( hundred > 0)
	{
		arab_0_99_to_english(hundred, str_tmp);
		sprintf(str, "%s hundred", str_tmp);
		if (arab != 0)
		{
			strcat(str, " and ");
			arab_0_99_to_english(arab, str_tmp);
			strcat(str, str_tmp);
		}
	}
	else
	{
		arab_0_99_to_english(arab, str_tmp);
		sprintf(str, "%s", str_tmp);
	}

	return 0;
}


static int arab_0_99_to_english(int arab, char *str)
{
	int tens;

	tens = arab / 10;
	arab -= tens*10;

	if (tens == 0)
	{
		switch (arab)
		{
			case 1:
				sprintf(str, "%s", dn.str + dn.sm_0_9[0]);
				break;
			case 2:
				sprintf(str, "%s", dn.str + dn.sm_0_9[1]);
				break;
			case 3:
				sprintf(str, "%s", dn.str + dn.sm_0_9[2]);
				break;
			case 4:
				sprintf(str, "%s", dn.str + dn.sm_0_9[3]);
				break;
			case 5:
				sprintf(str, "%s", dn.str + dn.sm_0_9[4]);
				break;
			case 6:
				sprintf(str, "%s", dn.str + dn.sm_0_9[5]);
				break;
			case 7:
				sprintf(str, "%s", dn.str + dn.sm_0_9[6]);
				break;
			case 8:
				sprintf(str, "%s", dn.str + dn.sm_0_9[7]);
				break;
			case 9:
				sprintf(str, "%s", dn.str + dn.sm_0_9[8]);
				break;
			case 0:
				sprintf(str, "%s", dn.str + dn.sm_0_9[9]);
				break;
		}
	}
	else if (tens == 1)
	{
		switch (arab)
		{
			case 1:
				sprintf(str, "%s", dn.str + dn.sm_10_19[1]);
				break;
			case 2:
				sprintf(str, "%s", dn.str + dn.sm_10_19[2]);
				break;
			case 3:
				sprintf(str, "%s", dn.str + dn.sm_10_19[3]);
				break;
			case 4:
				sprintf(str, "%s", dn.str + dn.sm_10_19[4]);
				break;
			case 5:
				sprintf(str, "%s", dn.str + dn.sm_10_19[5]);
				break;
			case 6:
				sprintf(str, "%s", dn.str + dn.sm_10_19[6]);
				break;
			case 7:
				sprintf(str, "%s", dn.str + dn.sm_10_19[7]);
				break;
			case 8:
				sprintf(str, "%s", dn.str + dn.sm_10_19[8]);
				break;
			case 9:
				sprintf(str, "%s", dn.str + dn.sm_10_19[9]);
				break;
			case 0:
				sprintf(str, "%s", dn.str + dn.sm_10_19[0]);
				break;
		}
	}
	else if (tens > 1)
	{
		switch (tens)
		{
			case 2:
				sprintf(str, "%s-", dn.str + dn.sm_20_90[0]);
				break;
			case 3:
				sprintf(str, "%s-", dn.str + dn.sm_20_90[1]);
				break;
			case 4:
				sprintf(str, "%s-", dn.str + dn.sm_20_90[2]);
				break;
			case 5:
				sprintf(str, "%s-", dn.str + dn.sm_20_90[3]);
				break;
			case 6:
				sprintf(str, "%s-", dn.str + dn.sm_20_90[4]);
				break;
			case 7:
				sprintf(str, "%s-", dn.str + dn.sm_20_90[5]);
				break;
			case 8:
				sprintf(str, "%s-", dn.str + dn.sm_20_90[6]);
				break;
			case 9:
				sprintf(str, "%s-", dn.str + dn.sm_20_90[7]);
				break;
		}

		switch (arab)
		{
			case 1:
				strcat(str, dn.str + dn.sm_0_9[0]);
				break;
			case 2:
				strcat(str, dn.str + dn.sm_0_9[1]);
				break;
			case 3:
				strcat(str, dn.str + dn.sm_0_9[2]);
				break;
			case 4:
				strcat(str, dn.str + dn.sm_0_9[3]);
				break;
			case 5:
				strcat(str, dn.str + dn.sm_0_9[4]);
				break;
			case 6:
				strcat(str, dn.str + dn.sm_0_9[5]);
				break;
			case 7:
				strcat(str, dn.str + dn.sm_0_9[6]);
				break;
			case 8:
				strcat(str, dn.str + dn.sm_0_9[7]);
				break;
			case 9:
				strcat(str, dn.str + dn.sm_0_9[8]);
				break;
		}

	}

	return 0;
}



