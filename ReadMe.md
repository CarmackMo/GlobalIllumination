# Global Illumination

## Introduction

This is a simple global illuminator developed based on the principle outlined in *Chapter 4: Ray Tracing* from the book *Fundamentals of Computer Graphics: Second Edition*. 

Based on the theories presented in the book, a basic **ray tracing** algorithm was implemented for rendering and the **Phong-Blinn shading model** is adopted for shading. This global illuminator is developed by C/C++


Read this in other languages:
[简体中文](https://blog.csdn.net/qq_29169285/article/details/117327116)

The outcome of the global illumination can be seen in the following image. There are five spheres in a closed room (The wall that at the same position of users' screen has no color). The light source is at the top of the room. The sphere on the right-top corner and the two spheres on the right-down corner use smooth material, while the rest of the spheres use rough material.

![Outcome](./Outcome.jpg)




<br></br>

## Catalog

+ [Ray Tracing Basic](#RayTracing)

+ [Initialize Ray](#InitializeRay)




<br></br>
<a id="RayTracing"></a>

# Ray Tracing Basic

To explain ray tracing, it's first necessary to understand how light forms images. Taking photography as an example, the process of forming a photograph can be simplified as follows: A light source emits countless rays in all directions. If these rays encounter an object in their path, they will reflect or refract at the point of intersection and then continue in the direction of reflection. A portion of the light rays emitted by the source will, after multiple reflections, ultimately land on the camera's photosensitive film. The totality of the light rays received by the film constitutes the final captured image.

In essence, photography maps a three-dimensional scene onto a two-dimensional image. Computer imaging is similar to photographic imaging, but computers cannot simulate the real-world process of photographic imaging. Even the smallest light source emits countless rays in all directions, making it impossible for computers to track and calculate the final destination of every single ray.

However, the number of rays involved in forming an image is limited and known, as the size of the imaging surface is fixed, and each pixel corresponds to a single ray. Therefore, the number of rays is equal to the number of pixels. If we reverse the process of photographic imaging, starting from the imaging surface and retracing the path of the rays back to the source, we can simulate the imaging process in reverse. Although computationally intensive, this process, known as 'Ray Tracing,' is within the capabilities of modern computers.

## Ray Tracing Algorithm

Based on the ray tracing principles described above, we can derive the proposed framework for ray tracing, summarized in pseudo-code as:

```java
for each pixel do :
    compute viewing ray
    if (ray hits an object) then :
        get hit object ID
        compute reflected ray 
        evaluate shading model
        recursively trace ray back until stop
        set pixel to shading color
    else :
        set shading color to background color
```

And the flowchat of the ray tracing process is like:

<img src="./Documents/Images/RayTracing.png" width="750px" >





<br></br>
<a id="InitializeRay"></a>

# Initialize Ray

To generate rays, a mathematical representation of the ray is essential. A ray is a directed line originating from a light source. Based on this principle, any ray in mathematics can be represented by its origin point and direction of propagation. The following formula can be derived:

$$\vec{P(t)} = \vec{e} + t \cdot \vec{d}  \qquad  t\in(0,\infty) \tag{1}$$

Where $\vec{e}$ is the three-dimensional coordinate of the ray's origin, $\vec{d}$ is the direction vector of the ray, and $t$ is a scalar representing the distance from the origin point along the direction $\vec{d}$. This formula represents a point on the path of the ray at a distance $t$ from the origin, and the total collection of all such points along the path constitutes the ray itself.

With the mathematical expression for a ray established, we can then calculate the initial position and direction of the ray. However, these values are not arbitrarily set; determining them involves another crucial aspect - the issue of viewpoint.

In the field of computer graphics, there are primarily two types of viewpoints: **Orthographic View** (also known as Parallel View) and **Perspective View**. In an Orthographic View, the imaging of objects is mapped onto corresponding pixels in a way that is perpendicular to the imaging surface. Therefore, all initial rays have the same direction perpendicular to the imaging surface, with their origins at their respective pixels on the imaging surface.

In contrast, the Perspective View is more akin to human eye imaging. All initial rays originate from a single point, often referred to as the **Viewing Point**. The imaging surface is located in front of the Viewing Point, and all initial rays emanate from this point, passing through their respective pixels on the imaging surface. Therefore, in a Perspective View, the directions of the rays are varied and radiate outward. The differences in the initial rays between these two viewpoints can be seen in the comparison illustrated below:

<img src="./Documents/Images/OrthographicAndPerspective.png" width="750px" >

<center><font size=3 color=grey>Figure 2: Orthographic view and perspective view </font></center>