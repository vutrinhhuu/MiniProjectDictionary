#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include "btree.h"


BTA *data;
GtkWidget *textView,*mainwindow;
GtkTextBuffer *buffer;


//Tim kiem va hien nghia

void *show_meaning(char *meaning){
  FILE *stream;
  GtkTextIter iter;
  char line[2000];
  static int created=0;
  // Ham chuyen 1 string tuong duong thanh 1 file
  stream=fmemopen(meaning,strlen(meaning)+1,"r");
  gtk_text_buffer_set_text(buffer,"",-1);
  gtk_text_buffer_get_start_iter(buffer,&iter);
  //Tag kieu chu
  if (!created){
    gtk_text_buffer_create_tag(buffer,"lmarg15","left_margin",15,NULL);
    gtk_text_buffer_create_tag(buffer,"lmarg30","left_margin",30,NULL);
    gtk_text_buffer_create_tag(buffer,"lmarg35","left_margin",35,NULL);
    gtk_text_buffer_create_tag(buffer,"bold","weight",PANGO_WEIGHT_BOLD,NULL);
    created=1;
  }
  //In nghia
  while (fgets(line,2000,stream)!=NULL){
    switch(line[0]){
    case '@':
      line[0]=' ';
      gtk_text_buffer_insert(buffer,&iter,"\n",-1);
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"bold",NULL);
      gtk_text_buffer_insert(buffer,&iter,"\n",-1);
      break;
    case '*':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg15",NULL);
      break;
    case '!':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg15",NULL);
      break;
    case '-':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg30",NULL);
      break;
    case '=':
      line[0]=' ';
      gtk_text_buffer_insert_with_tags_by_name(buffer,&iter,line,-1,"lmarg35",NULL);
      break;
    default:
      gtk_text_buffer_insert(buffer,&iter,line,-1);
      break;
    }
  }
}

gboolean display(GtkWidget *entryword,gpointer database){
  GtkWidget *dialog,*window=mainwindow;
  char word[50],mean[10000];
  int value;
  strcpy(word,(char *)gtk_entry_get_text(GTK_ENTRY(entryword)));
  if (strlen(word) == 0)
  {
    strcpy(mean,"Bạn chưa nhập từ vào ô tìm kiếm!");
  }
  else
  {
  	if (bfndky(data,word,(BTint *)&value)==0)
  	{
    	btsel(data,word,mean,10000,&value);
  	}
  	else
  	{
    strcpy(mean,"Xin lỗi! Từ này chưa có trong dữ liệu! \n\n Bạn có thể thêm từ này vào từ điển!\n");
  	}
  }
  show_meaning(mean);
}

int isPrefix(char *s,char *word){
  int len1,len2,i;
  len1=strlen(s);
  len2=strlen(word);
  if (len1 > len2) return 0;
  for (i=0;i<len1;i++){
    if (s[i]!=word[i])
      return 0;
    }
  return 1;
}

gboolean searchword(GtkWidget *entryword,GdkEvent *event,gpointer listword){
  GdkEventKey *key=(GdkEventKey *)event;
  GtkListStore *liststore=(GtkListStore *)listword;
  GtkTreeIter iter;
  int count=0,len;
  BTint value;
  char text[50],near_word[50];
  if (key->keyval==GDK_KEY_Tab)
  {
    strcpy(text,gtk_entry_get_text(GTK_ENTRY(entryword)));
    if (bfndky(data,text,&value)==QNOKEY)
    {
      // ham bnxtky tim key gan nhat voi key muon tim
      bnxtky(data,near_word,&value);
      if (isPrefix(text,near_word))
      {
  		gtk_entry_set_text(GTK_ENTRY(entryword),near_word);
  		gtk_editable_set_position(GTK_EDITABLE(entryword),strlen(near_word));
      }
      else return TRUE;
    }
    display(entryword,NULL);
    return TRUE;
  }
  else
  {
    //count=0;
    strcpy(text,gtk_entry_get_text(GTK_ENTRY(entryword)));
    if (key->keyval!=GDK_KEY_BackSpace)
    {
      	len=strlen(text);
      text[len]=key->keyval;
      text[len+1]='\0';
    }
    bfndky(data,text,&value);
    bnxtky(data,near_word,&value);
    gtk_list_store_clear(liststore);
    while ((isPrefix(text,near_word)!=0) && (count < 5))
    {
      gtk_list_store_append(liststore,&iter);
      gtk_list_store_set(liststore,&iter,0,near_word,-1);
      bnxtky(data,near_word,&value);
      count++;
    }
    return FALSE;
  }
}

//dialog
gboolean show_info(GtkWidget *widget,gpointer database){
  GdkPixbuf *pixbuf=gdk_pixbuf_new_from_file("info.png",NULL);
  GtkWidget *dialog=gtk_about_dialog_new();
  gtk_about_dialog_set_program_name(GTK_ABOUT_DIALOG(dialog),"Từ điển Anh-Việt");
  gtk_about_dialog_set_version(GTK_ABOUT_DIALOG(dialog),"Version 0.0");
  gtk_about_dialog_set_copyright(GTK_ABOUT_DIALOG(dialog),"(c) Việt Nhật C K60\n\nTrịnh Hữu Vũ\nDương Ngọc Sơn \nĐỗ Thị Hải \nNguyễn Thị Kiều Thương\n");
  gtk_about_dialog_set_comments(GTK_ABOUT_DIALOG(dialog),"Chương trình được tham khảo code từ trang zetcode.com và project của khóa K58\n");
  gtk_about_dialog_set_logo(GTK_ABOUT_DIALOG(dialog),pixbuf);
  g_object_unref(pixbuf),pixbuf=NULL;
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}

gboolean show_guide(GtkWidget *widget,gpointer database){
  GtkWidget *dialog,*window;
  window=mainwindow;
  dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_INFO,GTK_BUTTONS_OK,"1. Để tra từ, nhập từ sau đó nhấn Enter hoặc click vào nút Tra từ\n2. Để thêm từ, nhập từ và nghĩa sau đó click vào nút Thêm từ\n3. Để xóa từ, nhập từ và click vào nút Xóa từ\n4. Để thay đổi từ điển click vào từ điển bạn muốn tra\n5. Để đưa về màn hình ban đầu, click vào nút Khôi phục\n");
  gtk_window_set_title(GTK_WINDOW(dialog),"Hướng dẫn sử dụng");
  gtk_dialog_run(GTK_DIALOG(dialog));
  gtk_widget_destroy(dialog);
}



//Them tu va xoa tu

gboolean insert_word(GtkWidget *widget,gpointer entry){
  GtkWidget *dialog,*window=mainwindow;
  char word[50],meaning[10000];
  BTint value;
  GtkTextIter start,end;
  strcpy(word,(char*)gtk_entry_get_text(GTK_ENTRY(entry)));
  gtk_text_buffer_get_start_iter(buffer,&start);
  gtk_text_buffer_get_end_iter(buffer,&end);
  strcpy(meaning,(char*)gtk_text_buffer_get_text(buffer,&start,&end,0));
  if (strlen(word)==0)
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Bạn chưa nhập từ!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
  }
  else if (bfndky(data,word,&value)!=QNOKEY)
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Từ đã có trong cơ sở dữ liệu! Bạn muốn cập nhập nghĩa của từ này?");
    gtk_window_set_title(GTK_WINDOW(dialog),"Trùng từ");
    if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES)
        btupd(data,word,meaning,strlen(meaning)+1);
  }
  else
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Bạn muốn thêm từ này vào cơ sở dữ liệu?");
    gtk_window_set_title(GTK_WINDOW(dialog),"Xác nhận");
    if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES) 
    {
      btins(data,word,meaning,strlen(meaning)+1);
    }
  }
  gtk_widget_destroy(dialog);
}

gboolean delete_word(GtkWidget *widget, gpointer entry)
{
  GtkWidget *dialog,*window=mainwindow;
  char word[50];
  BTint value;
  strcpy(word,(char*)gtk_entry_get_text(GTK_ENTRY(entry)));
  if (strlen(word) == 0)
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Bạn chưa nhập từ!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
  }
  else if (bfndky(data,word,&value)==0)
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_QUESTION,GTK_BUTTONS_YES_NO,"Bạn muốn xóa từ này khỏi cơ sở dữ liệu?");
    gtk_window_set_title(GTK_WINDOW(dialog),"Xác nhận");
    if (gtk_dialog_run(GTK_DIALOG(dialog))==GTK_RESPONSE_YES) 
    {
      btdel(data, word);
      gtk_text_buffer_set_text(buffer,"",-1);
      gtk_entry_set_text(GTK_ENTRY(entry),"");
    }
  }
  else
  {
    dialog=gtk_message_dialog_new(GTK_WINDOW(window),GTK_DIALOG_DESTROY_WITH_PARENT,GTK_MESSAGE_WARNING,GTK_BUTTONS_OK,"Từ không có trong cơ sở dữ liệu!");
    gtk_window_set_title(GTK_WINDOW(dialog),"Lỗi!");
    gtk_dialog_run(GTK_DIALOG(dialog));
  }
  gtk_widget_destroy(dialog);
  return FALSE;
}

gboolean search_word(GtkWidget *button,gpointer database)
{
  display(GTK_WIDGET(database),NULL);
  return TRUE;
}

gboolean delete_event(GtkWidget *widget,GdkEvent *event,gpointer database)
{
    gtk_main_quit();
    return FALSE;
}

gboolean clearAll(GtkWidget *widget,gpointer database)
{
    gtk_text_buffer_set_text(buffer,"Xin chào!", -1);
    gtk_entry_set_text(GTK_ENTRY(database),"");
    return FALSE;
}

//button

void AnhViet(){
  btcls(data);
  data=btopn("AnhViet.dat",0,0);
}

void VietAnh(){
  btcls(data);
  data=btopn("VietAnh.dat",0,0);
}

GtkWidget* gtk_button_with_icon_and_label(char* label,char* fileimage){
  GtkWidget *button,*image;
  image=gtk_image_new();
  gtk_image_set_from_file(GTK_IMAGE(image),fileimage);
  button=gtk_button_new_with_label(label);
  gtk_button_set_image((GtkButton *)button,image);
  gtk_button_set_image_position((GtkButton *)button,GTK_POS_LEFT);
  return button;
}

//Tao icon ghep vao button
GdkPixbuf *create_pixbuf(const gchar *filename){
  GdkPixbuf *pixbuf;
  GError *error=NULL;
  pixbuf=gdk_pixbuf_new_from_file(filename,&error);
  if (!pixbuf){
    fprintf(stderr,"%s\n",error->message);
    g_error_free(error);
  }
  return pixbuf;
}

int main(int argc,char *argv[]){
  GtkWidget *window,*hbox;
  GtkWidget *button,*entry,*label,*title,*table,*wins;
  GtkWidget *ins,*del,*hlp,*info,*clear;
  GtkWidget *halign,*valign;
  // Khoi tao giao dien
  gtk_init(&argc,&argv);

  data=btopn("AnhViet.dat",0,FALSE);
  window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_position(GTK_WINDOW(window), GTK_WIN_POS_CENTER);
  gtk_widget_set_size_request (window, 800, 600);  
  gtk_window_set_title(GTK_WINDOW(window), "Từ điển Anh - Việt");
  gtk_container_set_border_width(GTK_CONTAINER(window), 15);
  gtk_window_set_icon(GTK_WINDOW(window),create_pixbuf("main.png"));
  g_signal_connect(window,"delete-event",G_CALLBACK(delete_event),NULL);
  mainwindow=window;
  // Ket thuc khoi tao


  // Tao bang chung cho tu dien

  table = gtk_table_new(10, 10, FALSE);
  gtk_table_set_col_spacings(GTK_TABLE(table), 3);
  gtk_table_set_row_spacing(GTK_TABLE(table), 0, 3);

  hbox = gtk_hbox_new (GTK_ORIENTATION_HORIZONTAL, 0);

  GtkEntryCompletion *completion=gtk_entry_completion_new();
  gtk_entry_completion_set_text_column(completion,0);
  GtkListStore *liststore=gtk_list_store_new(5,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING,G_TYPE_STRING);
  gtk_entry_completion_set_model(completion,GTK_TREE_MODEL(liststore));

  title = gtk_label_new("Tìm kiếm ");
  gtk_box_pack_start (GTK_BOX (hbox), title, 0, 1, 0);

  entry=gtk_search_entry_new();
  gtk_widget_set_size_request (entry, 400, 30);
  gtk_entry_set_completion(GTK_ENTRY(entry),completion);
  g_signal_connect(entry,"activate",G_CALLBACK(display),NULL);
  g_signal_connect(entry,"key-press-event",G_CALLBACK(searchword),liststore);
  gtk_box_pack_start (GTK_BOX (hbox), entry, 1, 1, 0);

  GtkSettings *default_settings=gtk_settings_get_default();
  g_object_set(default_settings,"gtk-button-images",TRUE,NULL);

  button=gtk_button_with_icon_and_label("","search.png");
  gtk_widget_set_size_request (button, 50, 20);
  g_signal_connect(button,"clicked",G_CALLBACK(search_word),(gpointer)entry);
  gtk_box_pack_start (GTK_BOX (hbox), button, 1, 1, 0);

  gtk_table_attach (GTK_TABLE (table), hbox, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 0, 0);



/*
  button=gtk_button_new_with_label("Từ điển Anh Việt");
  g_signal_connect(button,"clicked",G_CALLBACK(AnhViet),(gpointer)entry);
  gtk_box_pack_end(GTK_BOX(hbox),button,FALSE,FALSE,10);

  button=gtk_button_new_with_label("Từ điển Việt Anh");
  g_signal_connect(button,"clicked",G_CALLBACK(VietAnh),(gpointer)entry);
  gtk_box_pack_end(GTK_BOX(hbox),button,FALSE,FALSE,10);
*/
  
  // Tao cac button them, xoa, thong tin, khoi phuc

  ins=gtk_button_with_icon_and_label("  Thêm từ","add.png");
  g_signal_connect(ins,"clicked",G_CALLBACK(insert_word),(gpointer)entry);
  gtk_widget_set_size_request(ins, 50, 30);
  gtk_table_attach(GTK_TABLE(table), ins, 3, 4, 1, 2, 
      GTK_FILL, GTK_SHRINK, 1, 1);


  valign = gtk_alignment_new(0, 0, 0, 0);
  del=gtk_button_with_icon_and_label("   Xóa từ   ","del.png");
  g_signal_connect(del,"clicked",G_CALLBACK(delete_word),(gpointer)entry);
  gtk_widget_set_size_request(del, 100, 30);
  gtk_container_add(GTK_CONTAINER(valign), del);
  gtk_table_set_row_spacing(GTK_TABLE(table), 1, 2);
  gtk_table_attach(GTK_TABLE(table), valign, 3, 4, 2, 3, 
      GTK_FILL, GTK_FILL | GTK_EXPAND, 1, 1);

  hlp=gtk_button_with_icon_and_label(" Trợ giúp","help.png");
  g_signal_connect(hlp,"clicked",G_CALLBACK(show_guide),(gpointer)entry);
  gtk_widget_set_size_request(hlp, 50, 30);
  gtk_table_attach(GTK_TABLE(table), hlp, 3, 4, 3, 4, 
      GTK_FILL, GTK_SHRINK, 1, 1);

  halign = gtk_alignment_new(0, 1, 0, 0);
  info=gtk_button_with_icon_and_label(" Thông tin","info.png");
  g_signal_connect(info,"clicked",G_CALLBACK(show_info),(gpointer)entry);
  gtk_container_add(GTK_CONTAINER(halign), info);
  gtk_widget_set_size_request(info, 70, 30);
  gtk_table_set_row_spacing(GTK_TABLE(table), 3, 5);
  gtk_table_attach(GTK_TABLE(table), halign, 0, 1, 4, 5, 
      GTK_FILL, GTK_FILL, 0, 0);

  clear=gtk_button_with_icon_and_label(" Khôi phục","clear.png");
  g_signal_connect(clear,"clicked",G_CALLBACK(clearAll),(gpointer)entry);
  gtk_widget_set_size_request(clear, 70, 30);
  gtk_table_attach(GTK_TABLE(table), clear, 3, 4, 4, 5, 
      GTK_FILL, GTK_FILL, 0, 0);
//


  //Tao hop hien nghia

  hbox=gtk_box_new(GTK_ORIENTATION_HORIZONTAL,0);

  GtkWidget* scrolledWindow=gtk_scrolled_window_new(NULL,NULL);
  gtk_container_set_border_width(GTK_CONTAINER (scrolledWindow),10);
  gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolledWindow),GTK_POLICY_AUTOMATIC,GTK_POLICY_AUTOMATIC);

  textView=gtk_text_view_new();
  gtk_text_view_set_editable(GTK_TEXT_VIEW(textView), FALSE);
  gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(textView), FALSE);
  gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW(textView),GTK_WRAP_WORD);
  buffer=gtk_text_view_get_buffer(GTK_TEXT_VIEW(textView));
  gtk_text_buffer_set_text(buffer,"Xin chào !",-1);
  gtk_container_add(GTK_CONTAINER(scrolledWindow),textView);
  gtk_box_pack_start(GTK_BOX(hbox),scrolledWindow,TRUE,TRUE,0);
  gtk_table_attach(GTK_TABLE(table),hbox , 0, 2, 1, 4, 
      GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 1, 1);
  //

  gtk_container_add(GTK_CONTAINER(window), table);

  g_signal_connect(G_OBJECT(window), "destroy",
        G_CALLBACK(gtk_main_quit), G_OBJECT(window));

  gtk_container_add(GTK_CONTAINER(window), table);

  gtk_widget_show_all(window);
  gtk_main();
  btcls(data);
  return 0;
}