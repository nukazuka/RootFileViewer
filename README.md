# RootFileViewer
Draw all branch in TTrees, histograms, graphs and so on automatically

## Usage

	rfv [OPTIONS]

### OPTIONS

#### data selection

 option | detail |
 :----- | :----- |
 data          | You can specify data like following<br> --data="MC.root\|data/MC2.root" or --data="MC.root" --data="data/MC2.root <br>"\|" is used as a separator. |
 force         |  Ignore differences of structure of Root files |
 tree          |  you can specify the tree which will be drawn like following:<br> tree="MC_event" or tree=1.<br> If nothing is specifed, this program will ask. |
 except-branch |  not ready |

#### drawing setting


 option | detail |
 :----- | :----- |
 ratio |  Rarios of histogram (hist[i]/hist[0]) are drawn |
 both |  Both of normal histograms and rarios of histograms are drawn |
 cut |  cut is applied. |
 rand-cut |  not ready |
 norm |  Histgrams will be normalized. <br> To specify a value of nomarization, use norm-val |
 norm-val |  Specify a value of normalization |
 draw |  not ready |
 draw-ratio |  not ready |
 logx |  X axis is set log scale |
 logy |  Y axis is set log scale |
 xaxis-time |  not ready |

#### special

 option | detail |
 :----- | :----- |
 s-cut-DY2014 |  A random trigger cut is applied to only real data. |

#### misc

 option | detail |
 :----- | :----- |
 save |  not ready |
 out-dir |  Specify the directory which output file will be put in.          |
 option |  A list of options are drawn. |


