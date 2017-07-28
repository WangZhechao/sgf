# 如何制作数字雨特效

## 1. 前言

经典的黑客帝国的开头，有一波非常酷炫的数字雨特效，漆黑的背景加上绿色的数字看起来给人一种高大上的感觉，这篇文章就是介绍如何使用C语言制作这一特效的教程。

## 2. 结构

简单的分析一下，发现：

+ 特效是由若干列的字符串组成。
+ 每个字符串包含不定长的字符。
+ 字符的亮度由下到上依次递减。
+ 字符串快速从上到下移动直到超出屏幕为止。

根据分析，可以抽象出一个字符串的类型，包含速度、位置、字符串内容、字符串长度等基本属性，并可以声明几种常见的操作。

```C++
class StrLine
{
	friend class NumRain;

private:
	int _length;					//长度
	int _vy;						//速度
	int _wait_time;				//等待时间
	POINT _begin_pos;			//起始位置
	std::list<char> _num_list;	//数字列表

public:
	StrLine(int x);

public:
	CHAR genChar();				//生成字符
	void genLine();				//生成字符串
	void setPosX(int x);		//设置水平位置
};
```

接着定义一个类，负责组织管理显示这些字符串。

```C++
class DigitalRain
{
private:
	SIZE _font;
	SIZE _win;
	SGF::TextFormat* _textformat;

	std::vector<StrLine> _num_lines;

public:
	DigitalRain(const SIZE& win, const SIZE& font);
	~DigitalRain();

public:
	void genLines();
	bool createFont(SGF::Render* render);
	void destoryFont(SGF::Render* render);
	void update();
	void draw(SGF::Render* render);
};
```

## 3. 实现

代码的实现很简单，主要在于几个关键点：

+ 字符串的生成。
+ 字符串的更新。
+ 字符串的绘制。

对于字符串的生成，可以设置显示所有可视化的ASCII码：

```C++
CHAR StrLine::genChar()
{
	return CHAR(rand() % ('~' - '!') + '!');
}
```

对于字符串的更新，需要枚举所有字符串，根据字符串的速度更新下一次的位置，并且实时插入新的字符，删除旧的字符，只有这样才会有不断变换的效果。最后，还需要查看字符串是否超出屏幕范围，如果超出范围将其删除，并添加新的字符串。

```C++
void DigitalRain::update()
{
	std::vector<StrLine>::iterator itr;

	for (itr = _num_lines.begin(); itr != _num_lines.end(); ++itr)
	{
		//降低等待时间
		if (itr->_wait_time > 0)
		{
			itr->_wait_time--;
		}
		//删除旧的添加新的
		else
		{
			itr->_num_list.push_front(itr->genChar());
			itr->_num_list.pop_back();

			itr->_begin_pos.y += itr->_vy;
		}


		//越界
		if (itr->_begin_pos.y - (itr->_length * _font.cy) > _win.cy)
		{
			itr->genLine();
		}
	}
}
```

对于字符串的绘制，主要还是调用windows相关API显示即可，但是需要注意的一点，字符串的颜色是依次递减的。具体可以使用类似的语句动态设置字符的颜色：

```C++
render->SetTextColor(_textformat, RGB(0, 255 - 255 / (vitr->_length)*count, 0));
```

## 4. 小结

数字雨的特效实现非常简单，主要还是熟练运用windows相关API而已，具体逻辑更多的是为了更好的显示效果服务，特效代码可以从[github](http://wangzhechao.com/yuan-ma-xia-zai/)获得。

