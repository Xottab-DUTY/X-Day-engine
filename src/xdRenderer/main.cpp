////////////////////////////////
//
//  main.cpp
//  Author: Xottab_DUTY
//
////////////////////////////////
#include "pch.hpp"

#include "renderer.hpp"

extern "C" void XD_EXPORT InitializeRenderer()
{
    XDay::GEnv.Render = &XDay::Renderer::r_impl;
}
