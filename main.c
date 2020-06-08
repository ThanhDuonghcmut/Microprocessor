#include <string.h> // dinh kem thu vien chuoi
#include <at89x51.h> // dinh kem thu vien at89x51
#include <stdio.h>
#include <math.h>

#define pi 3.14159265
#define ma_loi_1 20000
#define ma_loi_2 20001
#define loi_1 "Sai cu phap!"
#define loi_2 "Loi tinh toan!"
#define khoangtrang "    "

sfr LCDdata=0xA0; // dung P2 lam port du lieu
sbit RS=P3^1; // chan RS noi voi P3.1
sbit EN=P3^2; // chan EN noi voi P3.2
	
sbit c1=P1^0;
sbit c2=P1^1;
sbit c3=P1^2;
sbit c4=P1^3;
	
sbit h1=P1^4;
sbit h2=P1^5;
sbit h3=P1^6;
sbit h4=P1^7;
sbit h5=P3^0;

unsigned char LCD_code[16]={'1','2','3','4','5','6','7','8','9','0',' ',' ','+','-','*','/'};

// phat hien co nut nhan hay khong
unsigned char KeyPressed()
{
	h1=h2=h3=h4=h5=0;
	if((c1==0)||(c2==0)||(c3==0)||(c4==0))
	return 1;
	return 0;
}	
// phat hien cot duoc nhan
unsigned char ScanCol()
{
	unsigned char c=0;
	if(c1==0) c=1;
	else if(c2==0) c=2;
	else if(c3==0) c=3;
	else if(c4==0) c=4;
	return c;
}
// phat hien hang duoc nhan
unsigned char ScanRow(int hang)
{
	h1=h2=h3=h4=h5=1;
	if(hang==1) h1=0;
	else if(hang==2) h2=0;
	else if(hang==3) h3=0;
	else if(hang==4) h4=0;
	else if(hang==5) h5=0;
	return 0;
}
// tinh toan vi tri phim duoc nhan
unsigned char Key_code()
{
	unsigned char i,Col;
	for(i=1;i<6;i++)
	{
		ScanRow(i);
		Col=ScanCol();
		if(Col>0) break; 
	}
		return ((i-1)*4 + Col);
}

void delayms(unsigned int time)
{
		while(time--);
}

void out2LCD(unsigned char x,bit y)
{
	unsigned char i=0;
	unsigned char tam=0;
	for(;i<8;i++)
	{
		tam>>=1;
		tam=tam|(x&0x80);
		x<<=1;
	}
	LCDdata=tam;// xuat gia tri ra Port du lieu
	EN=1;
	RS=y; // RS=0: che do LCD nhan lenh
	delayms(1000);
	EN=0;
}
void init_LCD(void)
{
	out2LCD(0x38,0);
	out2LCD(0x0E,0); 
	out2LCD(0x06,0); 
	out2LCD(0x01,0);
}
void gotoxy(unsigned char x,bit y)
{
	if(y==1)
	{
		out2LCD(192+x,0);
		return;
	}
	out2LCD(128+x,0);
}

void LCD_num(int d)
{
	if(d<0)
	{
		LCD_num(-d);
		out2LCD(0x10,0);
		out2LCD('-',1);
	}else if(d>0){
		while(d>0)
		{
			out2LCD(0x10,0);
			out2LCD(d%10+48,1);
			out2LCD(0x10,0);
			d=d/10;
		}
	}else if(d==0)
		out2LCD('0',1);
}

void LCD_string(char *str)
{
	unsigned char i=0;
	while(str[i]!=0)
	{
		out2LCD(str[i],1);
		i++;
	}
}


int cong_tru_nhan_chia(unsigned char so_phan_tu,unsigned char b[10])
{
	int sh1=0;
	int sh2=0;
	unsigned char dau=0;
	unsigned char i=0;
	bit soduong=1;
	if(b[so_phan_tu-1]>10)
		return ma_loi_1;
	if(b[0]==14)
	{
		soduong=0;
		i=1;
	}
	while((b[i]<11)&&(i<so_phan_tu))
	{
		sh1=sh1*10+b[i]%10;
		if(sh1>999)
			return ma_loi_2;
		i++;
	}
	sh1=sh1*(2*soduong-1);
	if(i==so_phan_tu)
		return sh1;
	dau=b[i];
	i++;
	if(b[i]==14)
	{
		soduong=0;
		i++;
	}else
		soduong=1;
	while((i<so_phan_tu)&&(b[i]<=10))
	{
		sh2=10*sh2+b[i]%10;
		if(sh2>999)
			return ma_loi_2;
		i++;
	}
	if(i!=so_phan_tu)
		return ma_loi_1;
	sh2=sh2*(2*soduong-1);
		switch(dau)
		{
			case 13:
				return sh1+sh2;
			case 14:
				return sh1-sh2;
			case 15:
				if((abs(sh1)>99)||(abs(sh2)>99))
					return ma_loi_2;
				return sh1*sh2;
			case 16:
				if((sh2==0)||(abs(sh1)>99)||(abs(sh2)>99))
					return ma_loi_2;
				return sh1*100/sh2;
			default:
				return sh1;
		}
	}

int tinh_luong_giac_va_can(unsigned char so_phan_tu,unsigned char b[10],bit ktsin,bit can,unsigned char dem)
{
	int sh=0;
	unsigned char i=0;
	bit soduong=1;
	if(dem!=1)
		return ma_loi_1;
	if(b[0]==14)
	{
		if(can)
			return ma_loi_2;
		soduong=0;
		i=1;
	}
	while((i<so_phan_tu)&&(b[i]<=10))
	{
		sh=sh*10+b[i]%10;
		if(sh>999)
			return ma_loi_2;
		i++;
	}
	if(i!=so_phan_tu)
		return ma_loi_1;
	sh=sh*(2*soduong-1);
	if(can)
		return (int) 1000*sqrt(sh);
	if(ktsin)
		return (int) 1000*sin(sh*pi/180);
		return (int) 1000*cos(sh*pi/180);
}

bit kiem_tra_loi(int ketqua)
{
	if(ketqua==ma_loi_1)
	{
		gotoxy(1,1);
		LCD_string(loi_1);
		return 1;
	}
	if(ketqua==ma_loi_2)
	{
		gotoxy(1,1);
		LCD_string(loi_2);
		return 1;
	}
	return 0;
}

void hieu_chinh_thap_phan(int ketqua,bit chia)
{
	unsigned char k=2;
	unsigned char i=0;
	unsigned char luythua10=1;
	if(chia)
		k=1;
	for(;i<k;i++)
		luythua10=10*luythua10;
	for(i=0;i<=k;i++)
	{
		if(ketqua<luythua10)
		{
			out2LCD(0x10,0);
			out2LCD('0',1);
			out2LCD(0x10,0);
		}
		luythua10=luythua10/10;
	}
}

// chuong trinh chinh
void main()
{
	unsigned char b[10];
	unsigned char i;
	unsigned char so_phan_tu=0;
	unsigned char k;
	unsigned char dem=0;
	int ketqua;
	bit luonggiac=0;
	bit sin;
	bit can=0;
	bit chia=0;
	bit cho_phep=1;
	init_LCD();
	while(1)
	{
		if(KeyPressed())
			delayms(20000);
		if(KeyPressed())
		{
			k=Key_code();
			if(!cho_phep)
			{
				while(k!=11)
					k=Key_code();
			}
			switch(k)
			{
				case 11:
					init_lCD();
					for(i=0;i<10;i++)
						b[i]=0;
					so_phan_tu=0;
					luonggiac=0;
					can=0;
					chia=0;
					dem=0;
					cho_phep=1;
					break;
				case 12:
					if(so_phan_tu==0)
					{
						if(dem!=0)
						{
							gotoxy((dem-1)*4,0);
							LCD_string(khoangtrang);
							gotoxy((dem-1)*4,0);
							dem--;
							break;
						}else
							break;
					}
					out2LCD(0x10,0);
					out2LCD(' ',1);
					out2LCD(0x10,0);
					so_phan_tu--;
					break;
				case 17:
					dem++;
					luonggiac=1;
					sin=1;
					LCD_string("sin(");
					break;
				case 18:
					dem++;
					luonggiac=1;
					sin=0;
					LCD_string("cos(");
					break;
				case 19:
					dem++;
					can=1;
					LCD_string("can(");
					break;
				case 20:
					if(so_phan_tu==0)
					{
						gotoxy(1,1);
						LCD_string("Nhap so vao");
						delayms(30000);
						gotoxy(1,1);
						LCD_string("           ");
						if(can||luonggiac)
							gotoxy(4,0);
						else
							gotoxy(0,0);
						break;
					}
					cho_phep=0;
					if(luonggiac||can)
					{
						LCD_string(")=");
						ketqua=tinh_luong_giac_va_can(so_phan_tu,b,sin,can,dem);
						if(kiem_tra_loi(ketqua))
							break;
						if(ketqua/1000==0)
							gotoxy(10,1);
						else
							gotoxy(11,1);
						LCD_num(ketqua/1000);
						ketqua=ketqua%1000;
						gotoxy(11,1);
						out2LCD('.',1);
						gotoxy(15,1);
						LCD_num(ketqua);
						hieu_chinh_thap_phan(ketqua,chia);
					}else if(chia)
					{
						ketqua=cong_tru_nhan_chia(so_phan_tu,b);
						out2LCD('=',1);
						if(kiem_tra_loi(ketqua))
							break;
						if(ketqua/100==0)
							gotoxy(11,1);
						else
							gotoxy(12,1);
						LCD_num(ketqua/100);
						ketqua=ketqua%100;
						gotoxy(12,1);
						out2LCD('.',1);
						gotoxy(15,1);
						LCD_num(ketqua);
						hieu_chinh_thap_phan(ketqua,chia);
					}else
					{
						ketqua=cong_tru_nhan_chia(so_phan_tu,b);
						out2LCD('=',1);
						if(kiem_tra_loi(ketqua))
						{
							break;
						}
						gotoxy(15,1);
						LCD_num(ketqua);
					}
					break;
				case 16:
					chia=1;
				default:
					out2LCD(LCD_code[k-1],1);
					b[so_phan_tu]=k;
					so_phan_tu++;
					break;
			}
		}
	}
}