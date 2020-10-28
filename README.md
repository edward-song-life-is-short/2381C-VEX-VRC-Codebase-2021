# 2381C Change Up Codebase

This repository was created as part of the VEX VRC Change Up Competition for the 2020-2021 competition season, using PROS V5 as our coding platform. Mainly, the programming aspect of the competition consists of coding:

- driver control (including macros for driving); and
- a reliable drive pathway for the autonomous sequence and programming skills.

The purpose of this project is to achieve the above criteria for the code successfully, which means to ensure reliability, simplicity and ensure that everything works well during the competition. This is the code that will be used by team 2381C for the competition season.

This repository belongs to team 2381C from Colonel By Secondary School (Ottawa, Canada). If you have any questions or concerns, please feel free to contact our team captain, Allen Tao, at allentao7@gmail.com.

## Table of Contents

1. [About](#about)
2. [Mathematics and Theory](#mathematicsandtheory)
3. [Odometry](#odometry)
4. [Motion](#motionprofiling)
5. [Contributing](#contributing)

<a name="headers"/>

<p align="center">
	<a href="https://www.youtube.com/watch?v=B-2Q9lkZ88g" target="_blank">
		<img src="http://img.youtube.com/vi/B-2Q9lkZ88g/0.jpg" 
		alt="2381C Change Up Robot Reveal" width="600" height="400" border="10" />
	</a>
</p>

## About

Team 2381C is a small team from Colonel By Secondary School, in Ottawa, Ontario. To learn more about us, you can contact our team captain allentao7@gmail.com or go through these links that you may find useful:

- https://2381.ca/ (organization website)
- https://vexdb.io/teams/view/2381C (our VEX DB profile)
- https://www.robotevents.com/teams/VRC/2381C (our RobotEvents profile)
- https://www.youtube.com/channel/UCHB6SeUT8tRKPbdgq9At18w (our YouTube channel)

## Mathematics and Theory
All the mathematics behind our custom motion algorithm state function, as well as odometry, is documented in extensive detail in our paper, which can be read here:
https://drive.google.com/file/d/1rogHSYbN24aIZ59ZjxgOQ-HCpfFKR6Uc/view?usp=sharing

## Odometry
Our main goal with programming this year was to implement robot position tracking to ameliorate our performance in the autonomous and programming skills periods. We’d like to thank team [5225A](thepilons.ca) for open-sourcing their odometry code from In The Zone, which was tremendously helpful for the creation of this codebase. Here’s a 115 points programming skills run we were able to achieve with odometry:

<p align="center">
	<a href="https://www.youtube.com/watch?v=R2jROKa9MXg" target="_blank">
		<img src="http://img.youtube.com/vi/R2jROKa9MXg/0.jpg" 
		alt="2381C Change Up Robot Reveal" width="600" height="400" border="10" />
	</a>
</p>

Our odometry loop relies on the modelling of robot motion as 2 distinct arcs, and mapping motions through vector motion on a 2d coordinate plane. For more insight into the function, please check the posTracking.cpp file in our /src folder. 

## Motion Profiling
Our autonomous routines rely on a combination of odometry and motion profiling, the latter of which is reliant on PID (Proportional Integral Derivative) loops to control our regular/strafe motion. 

## Contributing

As the competition season is still in progress, we are not accepting any community contributions. If you run through our code and see there are things we can improve, feel free to contact us or to open an issue. For new 2381C members, or for reference, here’s a link to the contribution guide: [contributing.md](docs/CONTRIBUTING.md)
