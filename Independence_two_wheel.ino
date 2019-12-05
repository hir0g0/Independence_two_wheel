//独立2輪車両のPWM制御のテストプログラム
//製作：2019年10月2日 Hiraga Teruaki
//2019年10月6日　改良：同じ処理を関数にする，コメントの追加


//右モータ用のピン設定
#define AIN1 7
#define AIN2 8
#define PWMA 9

//左モータ用のピン設定
#define AIN3 5
#define AIN4 4
#define PWMB 3

//コントローラからのデフォルトの入力値
#define kijun 513

//コントローラからの入力ピンの設定
const int JOYSTICK_X = A5;
const int JOYSTICK_Y = A6;

int val_x = 0; //x軸の読み取った値を入れる変数
int val_y = 0; //y軸

int reval_x = 0; //x軸の基準513を"0"に再定義
int reval_y = 0; //y軸の基準513を"0"に再定義

float carSpeed(int reval_x, int reval_y);
float velocity;

float moterSpeed_R(int reval_x, int inval_y); //右モーターに印加する電圧を計算する関数
float moterSpeed_L(int reval_x, int inval_y); //左モータに印加する電圧を計算する関数

void setup()
{
  pinMode(AIN1,OUTPUT);
  pinMode(AIN2,OUTPUT);
  pinMode(PWMA,OUTPUT);
  pinMode(AIN3,OUTPUT);
  pinMode(AIN4,OUTPUT);
  pinMode(PWMB,OUTPUT);

  Serial.begin(9600);
}

void loop()
{
  int mr, ml;
  
  val_x = analogRead(JOYSTICK_X);
  val_y = analogRead(JOYSTICK_Y);

  reval_x = val_x - kijun; //基準を0に直した値を入れる
  reval_y = val_y - kijun; //基準を0に直した値を入れる
  
  //モーターの回転停止
  if((val_x >=500) && (val_x <= 526) && (val_y >= 500) && (val_y <= 526)) // 基準からずれるとすぐに回転してしまうために，回転しない区間を設ける
  {
    digitalWrite(AIN1,LOW);
    digitalWrite(AIN2,LOW);
    digitalWrite(AIN3,LOW);
    digitalWrite(AIN4,LOW);
  }
  else if(val_y > 526)  //前進
  {
    digitalWrite(AIN1,HIGH);
    digitalWrite(AIN2,LOW);
    digitalWrite(AIN3,HIGH);
    digitalWrite(AIN4,LOW);

    float v = carSpeed(reval_y);

    mr = moterSpeed_R(reval_x, v);
    ml = moterSpeed_L(reval_x, v);

    analogWrite(PWMA, mr);
    analogWrite(PWMB, ml);
  }
  else if(val_y < 500) //後退
  {
    digitalWrite(AIN1,LOW);
    digitalWrite(AIN2,HIGH);
    digitalWrite(AIN3,LOW);
    digitalWrite(AIN4,HIGH);

    velocity = carSpeed(reval_y);

    mr = moterSpeed_R(reval_x, velocity);
    ml = moterSpeed_L(reval_x, velocity);

    analogWrite(PWMA, mr);
    analogWrite(PWMB, ml);
  }

  Serial.print("y_position: ");//シリアルモニタに表示する
  Serial.print(val_y);
  Serial.println();
  Serial.print("x_position: ");
  Serial.print(val_x);
  Serial.println();
  Serial.print("carSpeed: ");
  Serial.print(velocity);
  Serial.println();
  Serial.print("cycle ratio: ");
  Serial.print(reval_x);
  Serial.println();
  delay(500);
}

float carSpeed(int reval_y)
{
  float inval_y;

  if(reval_y < 0)
  {
    reval_y = - reval_y;
  }
  
  inval_y = (float)reval_y / 284.0 * 255.0; //284 = 797 - kijun，車体の速度
   
    if(inval_y < 1.0)
    {
      inval_y = 0.0; //もしその値が1.0以下なら動かないように0にする
    }
    else if(inval_y > 255.0)
    {
      inval_y = 255.0; //analogWrite関数に入れるので255を超えるときは255を無理やり入れる
    }

    return inval_y;
}

int moterSpeed_R(int reval_x, float velocity)
{
  int inval_R;
  
    if(reval_x >= 0) //右旋回かつ直進
    {
       inval_R = velocity * (-reval_x + 284) / 284; //carspeedに対する比を計算する
    }
    else if(reval_x < 0) //左旋回
    {
       inval_R = velocity; //carspeedをそのまま入れる
    }

    return (int)inval_R;
}

int moterSpeed_L(int reval_x, float velocity)
{
  int inval_L;
  
    if(reval_x >= 0) //右旋回かつ直進
    {
      inval_L = velocity; //carspeedをそのまま入れる
    }
    else if(reval_x < 0) //左旋回
    {
      inval_L = velocity * (reval_x + 284) / 284; //carspeedに対する比を計算する
    }

    return (int)inval_L;
}
