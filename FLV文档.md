## 第一周

### 视频直播流程

![捕获](.\src\捕获.PNG)



![20190529115324770165](.\src\20190529115324770165.png)

![8669504-a5c88572627769db](.\src\8669504-a5c88572627769db.webp)



### H.264码流格式

**H.264 的功能分为两层，即视频编码层（VCL）和网络提取层（NAL，Network Abstraction Layer）。**

VCL 数据即编码处理的输出，它表示被压缩编码后的视频数据序列。在 VCL 数据传输或存储之前，这些编码的 VCL 数据，先被映射或封装进 NAL 单元中。

**简单的说，VCL负责数据的编码压缩，NAL负责数据的封装传输。**

**H.264原始码流是一个一个的NALU组成的。**

![捕获1](.\src\捕获1.PNG)

***每个NALU之间都使用start code（起始码）分隔。***

start code根据NALU的不同类型为00 00 00 01 或者 00 00 01。

#### NALU结构

**每个 NALU包括一个头部信息（NAL header）和一个原始字节序列负荷（RBSP）。**

![20190305150031640](.\src\20190305150031640.jpg)

##### NAL header

其中的NAL header为一个字节。

![20190305150116674](.\src\20190305150116674.jpg)

类型为 7、8 的 NAL 单元中分别包含了序列参数集（SPS）和图像参数集（PPS），图像参数集和序列参数集在其它数据 NAL 单元的解码过程中作为参数使用，所以这两个数据对于解码图像数据非常重要。

##### **RBSP**

NAL 包将其负载数据存储在 RBSP(Raw Byte Sequence Payload) 中，RBSP 是一系列的 SODB(String Of Data Bits)。![2019030515031373](.\src\2019030515031373.jpg)

当我们拿到RBSP或SODB之后，就可以根据NALU Header中得到的NALU Type，去解析它们，重建图像。

#### H.264具体结构

![20190305150544846](.\src\20190305150544846.jpg)

**一张图片**可以包含**一个或多个分片(SLICE)**，而每**一个分片(SLICE)**又会被分为了**若干宏块(Macroblock)**。对于分片(SLICE)来说，有如下这些类型：![20190305150638332](.\src\20190305150638332.jpg)

**分片头（Slice Header）**中包含着分片类型、分片中的宏块类型、分片帧的数量以及对应的帧的设置和参数等信息，而分片数据中则是宏块，这里就是我们要找的存储像素数据的地方。

**宏块 (Macroblock) ** 是视频信息的主要承载者，因为它包含着每一个像素的亮度和色度信息。视频解码最主要的工作则是提供高效的方式从码流中获得宏块中的像素阵列。

#### 实例

![H264GIF](.\src\H264GIF.gif)



![捕获3](E:\第一阶段\src\捕获3.PNG)

> 起始码：00 00 00 01
>
> NALU Header: 06 (该NALU类型为补充增强信息单元 SEI)



![捕获4](E:\第一阶段\src\捕获4.PNG)

> 起始码：00 00 00 01
>
> NALU Header: 67 (SPS)
>
> 起始码：00 00 00 01
>
> NALU Header: 68 (PPS)



![捕获5](E:\第一阶段\src\捕获5.PNG)

> 起始码：00 00 00 01
>
> NALU Header: 65 (IDR帧的Slice)

### FLV文件格式

先上图：

![FLV](.\src\FLV.jpg)

FLV文件是由Flv Header 和Flv Body两大部分所组成的。

#### Flv Header

![FlvHeader](.\src\FlvHeader.jpg)

#### Flv Body

Flv Body由一系列的Tag组成，每个Tag又有一个preTagSize字段，标记着前面一个Tag的大小。

![FlvBody](.\src\FlvBody.jpg)

其中的Tag字段又有三种类型，分别是**Audio Tag（音频Tag），Video Tag（视频Tag），script Tag（又称Metadata Tag）。**

Tag又有其自己的结构。

#### Flv Tag

每个 Tag 都可以分为 Tag Header 和 Tag Data 两部分，针对不同类型的 Tag ，它们的 Tag Header 的格式是相同的，不同的是 Tag Data 部分。

![FlvTagData](.\src\FlvTagData.jpg)

#### Flv Tag Data

三种类型的Tag，每种都有自己的Tag Data格式

1. **Audio Tag Data**

   ![AudioTagData](.\src\AudioTagData.jpg)

如果SoundFormat=10，那么**音频数据**就是AACAUDIODATA。

AACAUDIODATA格式如下：

![AACAUDIODATA](.\src\AACAUDIODATA.jpg)

2. **Video Tag Data**

   ![VideoTagData](.\src\VideoTagData.jpg)

对于H.264数据来说，CodecID = 7。

当CodecID = 7时，视频数据就是**AVCVIDEOPACKET**格式。

**AVCVIDEOPACKET**  的格式如下：

![AVCVIDEOPACKET](.\src\AVCVIDEOPACKET.jpg)

- 当AVCPacketType = 0 时，Data部分就是AVCDecoderConfigurationRecord格式。

**AVCDecoderConfigurationRecord**的结构如下：

![AVCDecoderConfigurationRecord](.\src\AVCDecoderConfigurationRecord.jpg)

- 当AVCPacketType = 1 时，Data部分就是比较简单，如下面格式：

  ![Nalu](.\src\Nalu.jpg)

3. **Script Tag Data**

   该类型Tag又通常被称为MetadataTag，会放一些关于FLV视频和音频的元数据信息如：duration、width、height等。通常该类型Tag会跟在FileHeader后面作为第一个Tag出现，而且只有一个。

   一般一个flv文件由一个头部信息，一个script Tag，以及若干个video Tag和audio Tag组成。

   ![FlvStructure](.\src\FlvStructure.jpg)

Script Tag Data的结构如下：

![Script Tag](.\src\Script Tag.jpg)

对AMF包来说，第一个字节表示该AMF包的类型：

![AMFType](.\src\AMFType.jpg)

第一个AMF包：
第一个字节一般为0x02，表示字符串，第2-3个字节表示字符串的长度，一般为0x000A，后面跟的就是字符串，一般为"onMetaData"。

第二AMF包：
第一个字节为0x08，表示数组，第2-5个字节表示数组元素个数，后面跟着就是数组的元素，格式为：元素名长度（UI16） + 元素名（UI8[n]） + 元素的值(double，8字节)，该包的最后以“0x000009”结尾。其中常见的数组元素有下面这些：

![AMFMatrix](.\src\AMFMatrix.jpg)

### 实例解析

#### Flv Header：

![捕获6](.\src\捕获6.PNG)

> Signature：0x46('F')
> Signature：0x4c('L')
> Signature：0x56('V')
> Version：01
> Flags：01
>     TypeFlagsReserved [Bit 3-7]：0
>     TypeFlagsAudio [Bit 2]：0
>     TypeFlagsReserved [Bit 1]：0
>     TypeFlagsVideo [Bit 0]：1
> DataOffset：00 00 00 09
> preTagSize：00 00 00 00 // 第一个tag默认为0

#### Script Tag：

![捕获7](.\src\捕获7.PNG)

**Tag Header：**

> TagType：12（表示这是一个script tag）
> Datasize：00 00 9f（Tag Data 部分的大小）
> Timestamp：00 00 00（该Tag的时间戳）
> Timestamp_ex：00（时间戳的扩展部分）
> StreamID：：00 00 00（总是0）

**Tag data：**

因为TagType为0x12，所以这部分的数据为两个AMF包

第一个AMF包：

> type：02（表示字符串）
> stringLen：00 0a（字符串长度为10）
> string：6f 6e 4d 65 74 61 44 61 74 61（onMetaData）（10个）

第二个AMF包：

> type：08（表示数组）
> arrayNum：00 00 00 07（数组元素个数）
> stringLen：00 08（第一个数组元素字符串长度）
> string：64 75 72 61 74 69 6f 6e（duration）
> valType：00（数据类型，double型）
> val：40 30 09 ba 5e 35 3f 7d  （double型）
> stringLen：00 05
> string：77 69 64 74 68（width）
> ...
> end：00 00 09
>
> preTagSize：00 00 00 aa



**从下面开始就开始了H264数据的封装过程**

#### Video Tag

这部分封装着图像数据，也就是H.264的数据封装在这里。

H.264是由一个一个NALU组成，NALU的类型有（SPS、PPS、SEI、I帧的SLICE、非I帧的SLICE）

封装H.264就是将这些NALU一个一个取出来，然后封装成Tag。

其中，SPS与PPS必须封装在一个Tag中。

##### SPS和PPS的封装：

FLV文件：

![捕获8](.\src\捕获8.PNG)

 

H264文件：

![捕获9](.\src\捕获9.PNG)

**Tag Header：**

> TagType：09（Tag的类型，包括音频（0x08）、视频（0x09）、script data（0x12））
> Datasize：00 00 2e（Tag Data 部分的大小）
> Timestamp：00 00 00（时间戳，19ms）
> Timestamp_ex：00（时间戳的扩展部分）
> StreamID：00 00 00（总是0）

**Tag data：**

> FrameType | CodecID：17（keyframe | AVC）（视频tag的参数）

**AVCVideoPaket：（因为CodecID==7，所以VideoData为AVCVideoPaket）**

> AVCPaketType：00（ACVPacket的类型，0: AVC sequence header；1: AVC NALU；2: AVC end of sequence）
> CompositionTime：00 00 00

（因为ACVPaketType==0，所以Data=AVCDecoderConfigurationRecord）

**AVCDecoderConfigurationRecord：**

> configurationVersion：01
> AVCProfileIndication：64 （SPS第二个字节）
> profile_compatibility：00 （SPS第三个字节）
> AVCLevelIndication：1e （SPS第四个字节）
> lengthSizeMinusOne：ff 
> numOfSequenceParameterSets：e1（低五位为SPS的个数，计算方法为：numOfSequenceParameterSets & 0x1F=1）
> sequenceParameterSetLength：00 18（SPS的长度，24）
> sequenceParameterSetNALUnits：67 64 00 1e ac d9 40 a0 33 b0 11 00 00 03 02 47 00 00 6d 34 0f 16 2d 96（SPS）
> numOfPictureParameterSets：01（PPS的个数）
> pictureParameterSetLength：00 06（PPS的长度）
> pictureParameterSetNALUnits：68 eb e3 cb 22 c0（PPS）

##### I帧的封装：

FLV文件：

![捕获10](.\src\捕获10.PNG)

H264文件：

![捕获11](.\src\捕获11.PNG)

**Tag Header：**

> TagType：09（Tag的类型，包括音频（0x08）、视频（0x09）、script data（0x12））
> Datasize：00 35 78（Tag Data 部分的大小）
> Timestamp：00 00 42（时间戳）
> Timestamp_ex：00（时间戳的扩展部分）
> StreamID：00 00 00（总是0）

**Tag data：**

> FrameType | CodecID：17（key frame | AVC）（视频tag的参数）

因为CodecID==7，所以VideoData为AVCVideoPaket

**AVCVideoPaket：**

> AVCPaketType：**01**（ACVPacket的类型，0: AVC sequence header；1: AVC NALU；2: AVC end of sequence）
> CompositionTime：00 00 42

> NaluLen：00 00 35 6f
> NaluData：...

完了之后会有一个**previousTagSize：20 20 35 83**  

其他帧类型像SEI、非I帧的形式与I帧类似，只是对非I帧来说，它的**Tag data：**中的FrameType是2而不是1。

